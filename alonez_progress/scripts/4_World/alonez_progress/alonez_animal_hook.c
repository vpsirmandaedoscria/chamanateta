modded class AnimalBase
{
    protected PlayerBase m_AloneZLastAttacker;

    override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
    {
        super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
        if (!GetGame().IsServer()) return;
        if (!source) return;

        PlayerBase attacker = PlayerBase.Cast(source);
        if (!attacker)
            attacker = PlayerBase.Cast(source.GetHierarchyRootPlayer());

        if (attacker && attacker.GetIdentity())
            m_AloneZLastAttacker = attacker;
    }

    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);
        if (!GetGame().IsServer()) return;

        PlayerBase pb;
        if (killer)
        {
            pb = PlayerBase.Cast(killer);
            if (!pb)
            {
                EntityAI eai = EntityAI.Cast(killer);
                if (eai) pb = PlayerBase.Cast(eai.GetHierarchyRootPlayer());
            }
        }

        if (!pb && m_AloneZLastAttacker)
            pb = m_AloneZLastAttacker;

        if (pb && pb.GetIdentity())
        {
            string uid = pb.GetIdentity().GetPlainId();
            AloneZProgressSrv.AddProgress(pb, uid, AloneZCat.ANIMAL, 1);

            string animalType = this.GetType();
            array<AloneZSpecificObjective> specArr = AloneZStagesConfig.GetSpecific();
            for (int i = 0; i < specArr.Count(); i++)
            {
                if (specArr.Get(i).Name == animalType)
                {
                    AloneZProgressSrv.AddSpecificProgress(pb, uid, animalType, 1);
                    break;
                }
            }
        }
    }
}
