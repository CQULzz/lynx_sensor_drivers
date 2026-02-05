#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Event_dispatcher.h"
#include "Colossus_TCP_events.h"

using namespace Navtech;
using namespace Navtech::Utility;
using namespace std;

// Simple Abstract Data Type (ADT) for testing
//
class ADT { 
public:
    ADT() = default;
    ADT(int i) : value { i }
    {
    }

    virtual void operator()() const 
    { 
    }

    int value { };
};


class ADT_derived : public ADT {
public:
    using ADT::ADT;

    void operator()() const 
    {     
    }
};


// ------------------------------------------------------------------------------------
// Random event handler functions, for testing
//
void str_func(const std::string&)
{
}


void int_func(int)
{
}


void adt_func(const ADT&)
{
}


class Application {
public:
    void do_stuff(const std::string& s)
    {
        value = s;
    }

    void no_param()
    {
        value = "No parameter";
    }

    std::string value { };
};


void void_func()
{
}


// Event for testing
//
enum class Test_event { go, stop, wait, reset, finish };

template <Test_event> struct Test_event_traits { };
template <> struct Test_event_traits<Test_event::go>     { using Parameter = int; }; 
template <> struct Test_event_traits<Test_event::stop>   { using Parameter = std::string; }; 
template <> struct Test_event_traits<Test_event::wait>   { using Parameter = void; };
template <> struct Test_event_traits<Test_event::reset>  { using Parameter = void; };
template <> struct Test_event_traits<Test_event::finish> { using Parameter = ADT; }; 


// Type alias to simplify code
//
using Test_event_dispatcher = Dispatcher<Test_event, Test_event_traits>;


class GivenAnEventDispatcher : public ::testing::Test {
protected:
    GivenAnEventDispatcher() = default;
};


TEST_F(GivenAnEventDispatcher, AnEmptyHandlerCanBeConstructed)
{
    Event_handler<> ev_handler { };
}


TEST_F(GivenAnEventDispatcher, AHandlerCanHaveAScalarParameter)
{
    int result { };

    Test_event_dispatcher   dispatcher { };
    Event_handler<int>      ev_handler { [&result](int i) { result = i; } };

    dispatcher.attach_to<Test_event::go>(ev_handler);
    dispatcher.notify<Test_event::go>(100);

    ASSERT_EQ(result, 100);
}


TEST_F(GivenAnEventDispatcher, AHandlerCanHaveAStringParameter)
{
    string result { };

    Test_event_dispatcher   dispatcher { };
    Event_handler<string>   ev_handler { [&result](const string& s) { result = s; } };

    dispatcher.attach_to<Test_event::stop>(ev_handler);
    dispatcher.notify<Test_event::stop>("Test passed");

    ASSERT_EQ(result, "Test passed");
}


TEST_F(GivenAnEventDispatcher, AHandlerCanHaveAClassParameter)
{
    ADT result { };

    Test_event_dispatcher   dispatcher { };
    Event_handler<ADT>      ev_handler { [&result](const ADT& adt) { result = adt; } };

    dispatcher.attach_to<Test_event::finish>(ev_handler);
    dispatcher.notify<Test_event::finish>(ADT { 100 });

    ASSERT_EQ(result.value, 100);
}


TEST_F(GivenAnEventDispatcher, AHandlerCanHaveNoParameter)
{
    int result { };

    Test_event_dispatcher   dispatcher { };
    Event_handler<>         ev_handler { [&result]() { result = 1; } };

    ASSERT_EQ(result, 0);

    dispatcher.attach_to<Test_event::wait>(ev_handler);
    dispatcher.notify<Test_event::wait>();

    ASSERT_EQ(result, 1);
}


TEST_F(GivenAnEventDispatcher, AHandlerCanBeBoundToAMemberFunction)
{
    Application app { };

    Test_event_dispatcher       dispatcher { };
    Event_handler<std::string>  ev_handler { bind(&Application::do_stuff, &app, placeholders::_1) };

    ASSERT_TRUE(app.value.empty());

    dispatcher.attach_to<Test_event::stop>(ev_handler);
    dispatcher.notify<Test_event::stop>("Test passed");

    ASSERT_EQ(app.value, "Test passed");
}


TEST_F(GivenAnEventDispatcher, AHandlerCanBeBoundToAVoidMemberFunction)
{
    Application app { };

    Test_event_dispatcher       dispatcher { };
    Event_handler<>  ev_handler { bind(&Application::no_param, &app) };

    ASSERT_TRUE(app.value.empty());

    dispatcher.attach_to<Test_event::wait>(ev_handler);
    dispatcher.notify<Test_event::wait>();

    ASSERT_EQ(app.value, "No parameter");
}


TEST_F(GivenAnEventDispatcher, AHandlerCanBeRebound)
{
    Application app { };
    string result { };

    Test_event_dispatcher      dispatcher { };
    Event_handler<std::string> ev_handler { bind(&Application::do_stuff, &app, placeholders::_1) };

    ASSERT_TRUE(app.value.empty());

    dispatcher.attach_to<Test_event::stop>(ev_handler);
    dispatcher.notify<Test_event::stop>("Test passed");

    result = app.value;
    ASSERT_EQ(result, "Test passed");

    ev_handler = [&result](const string& s) { result = s; };
    dispatcher.notify<Test_event::stop>("Another test passed");

    ASSERT_EQ(result, "Another test passed");
}


TEST_F(GivenAnEventDispatcher, AnEventHandlerCanAttachToMultipleEvents)
{
    int result { };

    Test_event_dispatcher dispatcher { };
    Event_handler<> handler { [&result] { ++result; } };

    dispatcher.attach_to<Test_event::wait>(handler);
    dispatcher.attach_to<Test_event::reset>(handler);

    dispatcher.notify<Test_event::wait>();
    ASSERT_EQ(result, 1);

    dispatcher.notify<Test_event::reset>();
    ASSERT_EQ(result, 2);
}



// These tests require the network
// message handling code.
//
TEST_F(GivenAnEventDispatcher, TestTheStreamServerReceiveEvent)
{
    using namespace Networking::Colossus_protocol::TCP;

    Event_dispatcher dispatcher { };

    Message result { };

    Event_handler<Message> on_receive { 
        [&result](const Message& r)
        {
            result = r;
        }
    };

    dispatcher.attach_to<Event::received_message>(on_receive);

    Message test_msg { "192.168.2.1"_ipv4, 10 };

    dispatcher.notify<Event::received_message>(test_msg);

    ASSERT_EQ(result.id(), 10);
    ASSERT_EQ(result.ip_address().to_string(), "192.168.2.1");
}


TEST_F(GivenAnEventDispatcher, TestTheStreamServerSendEvent)
{
    using namespace Networking::Colossus_protocol::TCP;

    Event_dispatcher dispatcher { };

    Message result { };

    Event_handler<Message> on_send { 
        [&result](const Message& s)
        {
            result = s;
        }
    };

    dispatcher.attach_to<Event::send_message>(on_send);


    Message test_msg {
        std::vector<std::uint8_t> { 
            0x00, 0x01, 0x03, 0x03, 0x09, 0x09, 0x0F, 0x0F, 
            0x1F, 0x1F, 0x6F, 0x6F, 0x8F, 0x8F, 0xFE, 0xFE
        }
    };

    dispatcher.notify<Event::send_message>(test_msg);

    auto result_buffer = result.relinquish();

    ASSERT_EQ(result_buffer[0], 0x00);
    ASSERT_EQ(result_buffer[15], 0xFE);
    ASSERT_EQ(result_buffer.size(), 16);
}

