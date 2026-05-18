// ============================================================================
// AloneZ Bots — Controle de Animacoes
// Gerencia transicoes entre animacoes de movimento, combate e morte
// ============================================================================

class AloneZBotAnimHandler
{
    protected AloneZBotEntity m_Bot;
    protected string m_CurrentMovement;
    protected string m_CurrentStance;
    protected bool m_InCombat;
    protected bool m_IsAiming;
    protected float m_TransitionTimer;
    protected float m_TransitionDuration;

    void AloneZBotAnimHandler(AloneZBotEntity bot)
    {
        m_Bot = bot;
        m_CurrentMovement = "IDLE";
        m_CurrentStance = "ERECT";
        m_InCombat = false;
        m_IsAiming = false;
        m_TransitionTimer = 0;
        m_TransitionDuration = 0.3;

        if (AloneZBotsConfig.GetAnimationSettings())
        {
            m_TransitionDuration = AloneZBotsConfig.GetAnimationSettings().TransitionSmoothing;
        }
    }

    void SetMovement(string speed, string stance)
    {
        if (!m_Bot)
            return;

        m_CurrentMovement = speed;
        m_CurrentStance = stance;

        // Aplica stance
        ApplyStance(stance);

        // Aplica velocidade de movimento
        ApplyMovementSpeed(speed);
    }

    void SetCombat(bool inCombat)
    {
        m_InCombat = inCombat;

        if (inCombat)
        {
            // Bot levanta a arma
            if (AloneZBotsConfig.GetAnimationSettings().UseAimAnimation)
            {
                SetAiming(true);
            }
        }
        else
        {
            SetAiming(false);
        }
    }

    void SetAiming(bool aim)
    {
        m_IsAiming = aim;

        if (!m_Bot)
            return;

        if (aim)
        {
            m_Bot.RaiseWeapon();
        }
        else
        {
            m_Bot.LowerWeapon();
        }
    }

    void PlayIdle()
    {
        if (!m_Bot)
            return;

        if (AloneZBotsConfig.GetAnimationSettings().UseIdleAnimation)
        {
            ApplyMovementSpeed("IDLE");
        }
    }

    void PlayDeath()
    {
        if (!m_Bot)
            return;

        if (AloneZBotsConfig.GetAnimationSettings().UseDeathAnimation)
        {
            // A animacao de morte e tratada nativamente pelo engine ao setar vida para 0
            m_Bot.SetHealth("", "Health", 0);
        }
    }

    void PlayFire()
    {
        if (!m_Bot)
            return;

        if (AloneZBotsConfig.GetAnimationSettings().UseFireAnimation)
        {
            // Garante que o bot esta com arma raised durante tiro
            m_Bot.RaiseWeapon();
        }
    }

    void PlayReload()
    {
        if (!m_Bot)
            return;

        if (AloneZBotsConfig.GetAnimationSettings().UseReloadAnimation)
        {
            // Reload e tratado pelo sistema de combate (BotFireWeapon)
            // Aqui apenas abaixa a arma brevemente para simular animacao
            m_Bot.LowerWeapon();
        }
    }

    protected void ApplyStance(string stance)
    {
        if (!m_Bot)
            return;

        // Stance e controlada internamente — o engine aplica via movimento do bot
        m_CurrentStance = stance;
    }

    protected void ApplyMovementSpeed(string speed)
    {
        if (!m_Bot)
            return;

        // Velocidade e controlada internamente — o engine aplica via AI path
        m_CurrentMovement = speed;
    }

    // Retorna velocidade atual como float
    float GetCurrentSpeedValue()
    {
        if (m_CurrentMovement == "WALK") return 1.0;
        if (m_CurrentMovement == "JOG") return 2.0;
        if (m_CurrentMovement == "SPRINT") return 3.0;
        return 0;
    }

    string GetCurrentMovement()
    {
        return m_CurrentMovement;
    }

    string GetCurrentStance()
    {
        return m_CurrentStance;
    }

    bool IsAiming()
    {
        return m_IsAiming;
    }

    bool IsInCombat()
    {
        return m_InCombat;
    }
}
