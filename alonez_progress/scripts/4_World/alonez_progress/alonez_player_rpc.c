modded class PlayerBase
{
    protected PlayerBase m_AloneZLastPVPAttacker;

    override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
    {
        super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
        if (!GetGame().IsServer()) return;
        if (!source) return;

        PlayerBase attacker = PlayerBase.Cast(source);
        if (!attacker)
            attacker = PlayerBase.Cast(source.GetHierarchyRootPlayer());

        if (attacker && attacker.GetIdentity() && attacker != this)
            m_AloneZLastPVPAttacker = attacker;
    }

    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);

        if (rpc_type == 848420) 
        {
            Param1<int> p;
            if (!ctx.Read(p)) return;
            AloneZUIState.Zed += p.param1;
            if (AloneZUIState.Zed < 0) AloneZUIState.Zed = 0;
        }
        else if (rpc_type == 848421)
        {
            Param1<int> pa;
            if (!ctx.Read(pa)) return;
            AloneZUIState.Anim += pa.param1;
            if (AloneZUIState.Anim < 0) AloneZUIState.Anim = 0;
        }
        else if (rpc_type == 848422) 
        {
            Param1<int> p2;
            if (!ctx.Read(p2)) return;
            AloneZUIState.Zed = p2.param1;
        }
        else if (rpc_type == 848423) 
        {
            Param1<int> pb2;
            if (!ctx.Read(pb2)) return;
            AloneZUIState.Anim = pb2.param1;
        }
        else if (rpc_type == 848424) 
        {
            Param5<int,int,int,int,int> st;
            if (!ctx.Read(st)) return;
            AloneZUIState.StepZed      = st.param1;
            AloneZUIState.StepAnim     = st.param2;
            AloneZUIState.StepTime     = st.param3;
            AloneZUIState.StepDistance = st.param4;
            AloneZUIState.StepDeath    = st.param5;
        }
        else if (rpc_type == 848425) 
        {
            Param1<int> pt;
            if (!ctx.Read(pt)) return;
            AloneZUIState.TimeMinutes += pt.param1;
            if (AloneZUIState.TimeMinutes < 0) AloneZUIState.TimeMinutes = 0;
        }
        else if (rpc_type == 848426) 
        {
            Param1<int> ps;
            if (!ctx.Read(ps)) return;
            AloneZUIState.TimeMinutes = ps.param1;
        }
        else if (rpc_type == 848427) 
        {
            Param1<int> pd;
            if (!ctx.Read(pd)) return;
            AloneZUIState.DistanceKm += pd.param1;
            if (AloneZUIState.DistanceKm < 0) AloneZUIState.DistanceKm = 0;
        }
        else if (rpc_type == 848428) 
        {
            Param1<int> psd;
            if (!ctx.Read(psd)) return;
            AloneZUIState.DistanceKm = psd.param1;
        }
        else if (rpc_type == 848429) 
        {
            Param1<int> pdth;
            if (!ctx.Read(pdth)) return;
            AloneZUIState.Deaths += pdth.param1;
            if (AloneZUIState.Deaths < 0) AloneZUIState.Deaths = 0;
        }
        else if (rpc_type == 848430) 
        {
            Param1<int> psdth;
            if (!ctx.Read(psdth)) return;
            AloneZUIState.Deaths = psdth.param1;
        }
        else if (rpc_type == 848431)
        {
            Param1<int> prc;
            if (!ctx.Read(prc)) return;
            AloneZUIState.RewardCount = prc.param1;
            if (AloneZUIState.RewardCount < 0) AloneZUIState.RewardCount = 0;
        }
        else if (rpc_type == 848432)
        {
            Param5<int,int,int,int,int> stg;
            if (!ctx.Read(stg)) return;
            AloneZUIState.StageZed   = stg.param1;
            AloneZUIState.StageAnim  = stg.param2;
            AloneZUIState.StageTime  = stg.param3;
            AloneZUIState.StageDist  = stg.param4;
            AloneZUIState.StageDeath = stg.param5;
        }
        else if (rpc_type == 848433)
        {
            Param5<int,int,int,int,int> mx;
            if (!ctx.Read(mx)) return;
            AloneZUIState.MaxStageZed   = mx.param1;
            AloneZUIState.MaxStageAnim  = mx.param2;
            AloneZUIState.MaxStageTime  = mx.param3;
            AloneZUIState.MaxStageDist  = mx.param4;
            AloneZUIState.MaxStageDeath = mx.param5;
        }
        else if (rpc_type == ALONEZ_RPC_NOTIFY)
        {
            Param3<string, string, int> pn;
            if (!ctx.Read(pn) || !pn) return;
            string nTitle = pn.param1;
            string nMsg = pn.param2;
            string fullMsg = "[" + nTitle + "] " + nMsg;
            GetGame().GetMission().OnEvent(ChatMessageEventTypeID, new ChatMessageEventParams(CCDirect, "", fullMsg, ""));
        }
        else if (rpc_type == 80001)
        {
            Param1<string> prpt;
            if (!ctx.Read(prpt) || !prpt) return;
            string msg = prpt.param1;
            TStringArray parts = new TStringArray;
            msg.Split("\n", parts);
            for (int ii = 0; ii < parts.Count(); ii++)
            {
                MessageStatus(parts[ii]);
            }
        }
    }

    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);
        if (!GetGame().IsServer()) return;

        PlayerBase killerPB;
        if (killer)
        {
            killerPB = PlayerBase.Cast(killer);
            if (!killerPB)
            {
                EntityAI eai = EntityAI.Cast(killer);
                if (eai) killerPB = PlayerBase.Cast(eai.GetHierarchyRootPlayer());
            }
        }

        if (!killerPB && m_AloneZLastPVPAttacker)
            killerPB = m_AloneZLastPVPAttacker;

        if (!killerPB || !killerPB.GetIdentity()) return;
        if (killerPB == this) return;
        if (!GetIdentity()) return;
        if (killerPB.GetIdentity().GetPlainId() == GetIdentity().GetPlainId()) return;

        string uid = killerPB.GetIdentity().GetPlainId();
        AloneZProgressSrv.AddProgress(killerPB, uid, AloneZCat.DEATH, 1);
    }
}
