// ============================================================================
// AloneZ Bots — Logica de Combate e Sistema de Accuracy
// Deteccao de alvos, mira, disparo com dispersao baseada em accuracy
// ============================================================================

class AloneZBotCombatHandler
{
    protected AloneZBotEntity m_Bot;
    protected ref AloneZRouteConfig m_RouteConfig;
    protected float m_ReactionTimer;
    protected float m_ShootTimer;
    protected float m_BurstCooldownTimer;
    protected float m_CurrentAccuracy;
    protected bool m_HasReacted;
    protected int m_BurstShotsRemaining;
    protected int m_BurstShotsFired;
    protected float m_CurrentReactionTime;
    protected Object m_LastTarget;

    void AloneZBotCombatHandler(AloneZBotEntity bot, AloneZRouteConfig routeConfig)
    {
        m_Bot = bot;
        m_RouteConfig = routeConfig;
        m_HasReacted = false;
        m_BurstShotsRemaining = 0;
        m_BurstShotsFired = 0;
        m_ReactionTimer = 0;
        m_ShootTimer = 0;
        m_BurstCooldownTimer = 0;
        m_CurrentAccuracy = 0;
        m_CurrentReactionTime = 0;
        m_LastTarget = null;
    }

    void EngageTarget(Object target, float deltaTime)
    {
        if (!target || !m_Bot)
            return;

        // Se alvo mudou, reseta reacao
        if (target != m_LastTarget)
        {
            m_LastTarget = target;
            m_HasReacted = false;
            m_ReactionTimer = 0;
            float rtMin = AloneZBotsConfig.GetAccuracySettings().ReactionTimeMin;
            float rtMax = AloneZBotsConfig.GetAccuracySettings().ReactionTimeMax;
            m_CurrentReactionTime = Math.RandomFloatInclusive(rtMin, rtMax);
        }

        // Tempo de reacao antes do primeiro tiro
        if (!m_HasReacted)
        {
            m_ReactionTimer += deltaTime;

            if (m_ReactionTimer < m_CurrentReactionTime)
            {
                // Durante reacao, bot vira para o alvo mas nao atira
                m_Bot.LookAtPosition(target.GetPosition());
                return;
            }

            m_HasReacted = true;
            m_ReactionTimer = 0;

            AloneZBotsLogger.LogDebug("COMBAT_START", "Bot '" + m_Bot.GetBotName() + "' reagiu ao alvo. Iniciando engajamento.");
        }

        // Vira para o alvo
        m_Bot.LookAtPosition(target.GetPosition());

        // Calcula accuracy para este frame
        m_CurrentAccuracy = CalculateAccuracy(target);

        // Sistema de burst fire
        if (AloneZBotsConfig.GetAccuracySettings().BurstFireEnabled)
        {
            UpdateBurstFire(target, deltaTime);
        }
        else
        {
            UpdateSingleFire(target, deltaTime);
        }

        // Verifica se precisa recarregar
        CheckReload();
    }

    protected void UpdateBurstFire(Object target, float deltaTime)
    {
        if (m_BurstShotsRemaining > 0)
        {
            // Em rajada — atira rapido
            m_ShootTimer += deltaTime;
            float burstRate = 0.1; // Intervalo entre tiros na rajada

            if (m_ShootTimer >= burstRate)
            {
                m_ShootTimer = 0;
                FireAtTarget(target);
                m_BurstShotsRemaining--;
                m_BurstShotsFired++;
            }
        }
        else
        {
            // Cooldown entre rajadas
            m_BurstCooldownTimer += deltaTime;
            float cdMin = AloneZBotsConfig.GetAccuracySettings().TimeBetweenShotsMin;
            float cdMax = AloneZBotsConfig.GetAccuracySettings().TimeBetweenShotsMax;
            float cooldown = Math.RandomFloatInclusive(cdMin, cdMax);

            if (m_BurstCooldownTimer >= cooldown)
            {
                m_BurstCooldownTimer = 0;
                m_BurstShotsFired = 0;

                // Nova rajada
                int bMin = AloneZBotsConfig.GetAccuracySettings().BurstMinShots;
                int bMax = AloneZBotsConfig.GetAccuracySettings().BurstMaxShots + 1;
                m_BurstShotsRemaining = Math.RandomInt(bMin, bMax);
            }
        }
    }

    protected void UpdateSingleFire(Object target, float deltaTime)
    {
        m_ShootTimer += deltaTime;
        float tbsMin = AloneZBotsConfig.GetAccuracySettings().TimeBetweenShotsMin;
        float tbsMax = AloneZBotsConfig.GetAccuracySettings().TimeBetweenShotsMax;
        float timeBetweenShots = Math.RandomFloatInclusive(tbsMin, tbsMax);

        if (m_ShootTimer >= timeBetweenShots)
        {
            m_ShootTimer = 0;
            FireAtTarget(target);
        }
    }

    float CalculateAccuracy(Object target)
    {
        // Base accuracy (da rota ou global)
        float minAcc, maxAcc;
        if (m_RouteConfig.BotAccuracy.OverrideGlobal)
        {
            minAcc = m_RouteConfig.BotAccuracy.MinAccuracy;
            maxAcc = m_RouteConfig.BotAccuracy.MaxAccuracy;
        }
        else
        {
            minAcc = AloneZBotsConfig.GetAccuracySettings().GlobalMinAccuracy;
            maxAcc = AloneZBotsConfig.GetAccuracySettings().GlobalMaxAccuracy;
        }

        float baseAccuracy = Math.RandomFloatInclusive(minAcc, maxAcc);

        // Modificador de distancia
        float distance = vector.Distance(m_Bot.GetPosition(), target.GetPosition());
        float distModifier = 1.0;
        if (AloneZBotsConfig.GetAccuracySettings().AccuracyScaleWithDistance)
        {
            float falloff = AloneZBotsConfig.GetAccuracySettings().AccuracyDistanceFalloff;
            distModifier = Math.Clamp(1.0 - (distance * falloff), 0.1, 1.0);
        }

        // Modificador de stance
        float stanceModifier = 1.0;
        string stance = m_Bot.GetCurrentStanceStr();
        if (stance == "PRONE")
            stanceModifier = 1.2;
        else if (stance == "CROUCH")
            stanceModifier = 1.0;
        else // ERECT
            stanceModifier = 0.8;

        // Modificador de movimento
        float moveModifier = 1.0;
        float speed = m_Bot.GetCurrentSpeedValue();
        if (speed < 0.1)
            moveModifier = 1.0;      // Parado
        else if (speed < 1.5)
            moveModifier = 0.7;      // Andando
        else if (speed < 2.5)
            moveModifier = 0.4;      // Correndo
        else
            moveModifier = 0.2;      // Sprint

        float rawAccuracy = baseAccuracy * distModifier * stanceModifier * moveModifier;
        float finalAccuracy = Math.Clamp(rawAccuracy, 0.0, 1.0);

        return finalAccuracy;
    }

    protected void FireAtTarget(Object target)
    {
        if (!m_Bot || !target)
            return;

        // Vira para o alvo
        m_Bot.LookAtPosition(target.GetPosition());

        // Headshot chance
        bool isHeadshot = false;
        if (Math.RandomFloat01() < AloneZBotsConfig.GetAccuracySettings().HeadshotChance)
            isHeadshot = true;

        // Dispara com accuracy como probabilidade de acerto
        m_Bot.BotFireWeapon(target, m_CurrentAccuracy, isHeadshot);

        float fireDist = vector.Distance(m_Bot.GetPosition(), target.GetPosition());
        string fireMsg = "Bot '" + m_Bot.GetBotName() + "' disparou. Accuracy: " + m_CurrentAccuracy.ToString() + " Dist: " + fireDist.ToString() + "m";
        AloneZBotsLogger.LogDebug("COMBAT_FIRE", fireMsg);
    }

    protected void CheckReload()
    {
        if (!m_Bot)
            return;

        Weapon_Base weapon = Weapon_Base.Cast(m_Bot.GetHumanInventory().GetEntityInHands());
        if (weapon)
        {
            int muzzleIndex = weapon.GetCurrentMuzzle();
            if (weapon.IsChamberEmpty(muzzleIndex))
            {
                // Precisa recarregar
                if (m_Bot.GetAnimHandler())
                {
                    m_Bot.GetAnimHandler().PlayReload();
                }

                AloneZBotsLogger.LogDebug("COMBAT_START", "Bot '" + m_Bot.GetBotName() + "' recarregando arma.");
            }
        }
    }

    // Verifica se alvo esta no alcance de engajamento
    bool IsTargetInRange(Object target)
    {
        if (!target || !m_Bot)
            return false;

        float dist = vector.Distance(m_Bot.GetPosition(), target.GetPosition());
        return dist <= AloneZBotsConfig.GetGlobalSettings().BotEngageRange;
    }

    // Verifica se alvo esta fora do alcance de desengajamento
    bool IsTargetOutOfRange(Object target)
    {
        if (!target || !m_Bot)
            return true;

        float dist = vector.Distance(m_Bot.GetPosition(), target.GetPosition());
        return dist > AloneZBotsConfig.GetGlobalSettings().BotDisengageRange;
    }

    // Retorna accuracy atual
    float GetCurrentAccuracy()
    {
        return m_CurrentAccuracy;
    }

    void Reset()
    {
        m_HasReacted = false;
        m_ReactionTimer = 0;
        m_ShootTimer = 0;
        m_BurstCooldownTimer = 0;
        m_BurstShotsRemaining = 0;
        m_BurstShotsFired = 0;
        m_CurrentAccuracy = 0;
        m_CurrentReactionTime = 0;
        m_LastTarget = null;
    }
}
