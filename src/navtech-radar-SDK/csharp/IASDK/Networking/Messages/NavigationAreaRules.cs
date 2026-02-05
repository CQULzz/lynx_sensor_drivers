using Navtech.IASDK.Extensions;
using System.Collections.Generic;

namespace Navtech.IASDK.Networking.Messages;

/// <summary>
/// Navigation Area Rules
/// </summary>
public class NavigationAreaRules
{

    /// <summary>
    /// Enable or disable the health
    /// </summary>
    public bool EnableHealth { get; set; }

    /// <summary>
    /// Enable or disable fail safe mode
    /// </summary>
    public bool FailSafeMode { get; set; }

    /// <summary>
    /// Navigation Rules
    /// </summary>
    public List<NavigationAreaRule> Rules { get; init; }

    /// <summary>
    /// Construct a set of rules from the network message
    /// </summary>
    /// <param name="message">The network message</param>
    /// <param name="payload"></param>
    public NavigationAreaRules(TcpAreaRulesMessage message, byte[] payload)
    {
        EnableHealth = message.EnableHealth;
        FailSafeMode = message.FailSafeMode;
        Rules = new List<NavigationAreaRule>();
        var ruleCount = EnableHealth && message.RuleCount == 6 ? message.RuleCount - 1 : message.RuleCount;
        var dataOffset = 0;
        for (var i = 0; i < ruleCount; i++)
        {
            var ruleMessage = payload[dataOffset..].MarshalToObject<NavigationAreaRuleMessage>();
            var dataEnd = dataOffset + ruleMessage.RuleLength + 4;
            var pointData = payload[(dataOffset + NavigationAreaRuleMessage.Size)..(int)dataEnd];

            Rules.Add(new NavigationAreaRule(ruleMessage, pointData));

            dataOffset = (int)dataEnd;
        }
    }

    /// <summary>
    /// Default constructor
    /// </summary>
    public NavigationAreaRules()
    {

    }
}