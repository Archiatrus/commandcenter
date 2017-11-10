#include "Condition.h"
#include "CCBot.h"

Condition::Condition()
    : m_bot     (nullptr)
    , m_lhs     (nullptr)
    , m_rhs     (nullptr)
    , m_type    (ConditionTypes::Invalid)
    , m_op      (ConditionOperators::Invalid)
    , m_player  (0)
{

}

Condition::Condition(const json & j, CCBot & bot)
    : Condition()
{
    m_bot = &bot;

    BOT_ASSERT(j.is_array(), "Condition must be an array");

    // this is a condition with 2 sub-conditions and an operator
    if (j.size() == 3) 
    {
        BOT_ASSERT(j[1].is_string(), "Condition operator must be a string");

        m_lhs   = new Condition(j[0], bot);
        m_op    = GetOperator(j[1].get<std::string>());
        m_rhs   = new Condition(j[2], bot);
        m_type  = GetType(j[1].get<std::string>());
    }
    // this is a condition unary value with 'self' or 'enemy' followed by a unit type
    else if (j.size() == 2)
    {
        m_type  = ConditionTypes::Unary;

        BOT_ASSERT(j[0].is_string(), "First element of size 2 condition must be a string");
        BOT_ASSERT(j[1].is_string(), "Second element of size 2 condition must be a string");

        m_player = GetPlayer(j[0].get<std::string>());
        m_strValue  = j[1].get<std::string>();
    }
    // this is either an integer or a string denoting some fixed game value like time
    else if (j.size() == 1)
    {
        m_type  = ConditionTypes::Unary;

        if (j[0].is_number_integer())
        {
            m_intValue = j[0];
        }
        else if (j[0].is_string())
        {
            m_strValue = j[0].get<std::string>();
        }
        
        BOT_ASSERT(false, "Size 1 Condition must be int or string");
    }
}

int Condition::intEval() const
{
    if (m_type == ConditionTypes::BinaryInt)
    {
        switch (m_op)
        {
            case ConditionOperators::PLUS:   { return m_lhs->intEval() + m_rhs->intEval(); }
            case ConditionOperators::MINUS:  { return m_lhs->intEval() - m_rhs->intEval(); }
            case ConditionOperators::MULT:   { return m_lhs->intEval() * m_rhs->intEval(); }
            case ConditionOperators::DIV:    { return m_lhs->intEval() / m_rhs->intEval(); }
        }
        
    }
    else if (m_type == ConditionTypes::Unary)
    {
        
    }
    
    BOT_ASSERT(false, "Can't int evaluate this Condition");
    return 0;
}

bool Condition::eval() const
{
    if (m_lhs->m_type == ConditionTypes::BinaryBool)
    {
        switch (m_op)
        {
            case ConditionOperators::AND:    { return m_lhs->eval() && m_rhs->eval(); }
            case ConditionOperators::OR:     { return m_lhs->eval() || m_rhs->eval(); }    
        }
    }
    else if (m_lhs->m_type == ConditionTypes::BinaryComp)
    {
        switch (m_op)
        {
            case ConditionOperators::LT:     { return m_lhs->intEval() <  m_rhs->intEval(); }
            case ConditionOperators::GT:     { return m_lhs->intEval() >  m_rhs->intEval(); }
            case ConditionOperators::EQ:     { return m_lhs->intEval() == m_rhs->intEval(); }
            case ConditionOperators::NEQ:    { return m_lhs->intEval() != m_rhs->intEval(); }
            case ConditionOperators::LTE:    { return m_lhs->intEval() <= m_rhs->intEval(); }
            case ConditionOperators::GTE:    { return m_lhs->intEval() >= m_rhs->intEval(); }
        }
    }

    BOT_ASSERT(false, "Can't bool eval this Condition");
    return false;
}

int Condition::GetOperator(const std::string & op)
{
    if (op == "LT")     { return ConditionOperators::LT; }
    if (op == "GT")     { return ConditionOperators::GT; }
    if (op == "EQ")     { return ConditionOperators::EQ; }
    if (op == "NEQ")    { return ConditionOperators::NEQ; }
    if (op == "LTE")    { return ConditionOperators::LTE; }
    if (op == "GTE")    { return ConditionOperators::GTE; }
    if (op == "AND")    { return ConditionOperators::AND; }
    if (op == "OR")     { return ConditionOperators::OR; }
    if (op == "+")      { return ConditionOperators::PLUS; }
    if (op == "-")      { return ConditionOperators::MINUS; }
    if (op == "*")      { return ConditionOperators::MULT; }
    if (op == "/")      { return ConditionOperators::DIV; }

    BOT_ASSERT(false, "Invalid Condition operator: ", op.c_str());
    return ConditionOperators::Invalid;
}

int Condition::GetType(const std::string & op)
{
    if (op == "LT")     { return ConditionTypes::BinaryComp; }
    if (op == "GT")     { return ConditionTypes::BinaryComp; }
    if (op == "EQ")     { return ConditionTypes::BinaryComp; }
    if (op == "NEQ")    { return ConditionTypes::BinaryComp; }
    if (op == "LTE")    { return ConditionTypes::BinaryComp; }
    if (op == "GTE")    { return ConditionTypes::BinaryComp; }
    if (op == "AND")    { return ConditionTypes::BinaryBool; }
    if (op == "OR")     { return ConditionTypes::BinaryBool; }
    if (op == "+")      { return ConditionTypes::BinaryInt; }
    if (op == "-")      { return ConditionTypes::BinaryInt; }
    if (op == "*")      { return ConditionTypes::BinaryInt; }
    if (op == "/")      { return ConditionTypes::BinaryInt; }

    BOT_ASSERT(false, "Invalid Condition operator: ", op.c_str());
    return ConditionOperators::Invalid;
}

int Condition::GetPlayer(const std::string & player)
{
    std::string playerLower = player;
    std::transform(playerLower.begin(), playerLower.end(), playerLower.begin(), ::tolower);

    if (playerLower == "self") { return 0; }
    if (playerLower == "enemy") { return 1; }

    BOT_ASSERT(false, "Unknown Player string:", player.c_str());
    return 0;
}