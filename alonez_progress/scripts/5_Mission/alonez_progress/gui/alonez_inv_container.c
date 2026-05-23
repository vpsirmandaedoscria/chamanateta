class AloneZInvSkillContainer
{
    GridSpacerWidget m_Parent;
    WrapSpacerWidget m_Container;
    protected TextWidget m_RewardCounter;
    ref AloneZInvSkill m_SkillZed;
    ref AloneZInvSkill m_SkillAnim;
    ref AloneZInvSkill m_SkillTime; ref AloneZInvSkill m_SkillDist; ref AloneZInvSkill m_SkillDeath;

    void AloneZInvSkillContainer(Widget parent)
    {
        if (Class.CastTo(m_Parent, parent))
        {
            Widget root = GetGame().GetWorkspace().CreateWidgets("AloneZ_Progress/gui/layouts/alonez/alonez_inv_container.layout", m_Parent);
            if (root) root.SetName("AloneZInvRoot");
            m_Container = WrapSpacerWidget.Cast(root);
            m_RewardCounter = TextWidget.Cast(root.FindAnyWidget("AloneZRewardCounter"));
            if (m_RewardCounter) m_RewardCounter.SetText("Recompensas ganhas: 0");
            if (m_Container)
            {
                m_Container.SetPos(0, 0);
                m_SkillZed  = new AloneZInvSkill(m_Container);  m_SkillZed.SetLabel("Zumbis");           m_SkillZed.SetColor(ARGB(255,255,0,0));
                m_SkillZed.SetY(20);
                m_SkillAnim = new AloneZInvSkill(m_Container);  m_SkillAnim.SetLabel("Animais");           m_SkillAnim.SetColor(ARGB(255,0,255,0));
                m_SkillAnim.SetY(60);
                m_SkillTime = new AloneZInvSkill(m_Container);  m_SkillTime.SetLabel("Tempo no servidor"); m_SkillTime.SetColor(ARGB(255,0,128,255));
                m_SkillTime.SetY(100);
                m_SkillDist = new AloneZInvSkill(m_Container);  m_SkillDist.SetLabel("Km percorridos");        m_SkillDist.SetColor(ARGB(255,139,69,19));
                m_SkillDist.SetY(140);
                m_SkillDeath = new AloneZInvSkill(m_Container); m_SkillDeath.SetLabel("Players mortos"); m_SkillDeath.SetColor(ARGB(255,160,32,240));
                m_SkillDeath.SetY(180);
            }
        }
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.Update, 200, true);
    }

    string LevelText(string name, int stage, int maxStage, int val, int step)
    {
        if (stage >= maxStage) return name + " — LEVEL MAX";
        return name + " — Level " + (stage + 1).ToString() + " (" + val.ToString() + "/" + step.ToString() + ")";
    }

    void Update()
    {
        if (!m_SkillZed || !m_SkillAnim || !m_SkillTime || !m_SkillDist || !m_SkillDeath) return;

        if (AloneZUIState.StageZed >= AloneZUIState.MaxStageZed)
        { m_SkillZed.UpdateBar(1, 1); }
        else
        { m_SkillZed.UpdateBar(AloneZUIState.Zed, AloneZUIState.StepZed); }
        m_SkillZed.SetLabel(LevelText("Zumbis", AloneZUIState.StageZed, AloneZUIState.MaxStageZed, AloneZUIState.Zed, AloneZUIState.StepZed));

        if (AloneZUIState.StageAnim >= AloneZUIState.MaxStageAnim)
        { m_SkillAnim.UpdateBar(1, 1); }
        else
        { m_SkillAnim.UpdateBar(AloneZUIState.Anim, AloneZUIState.StepAnim); }
        m_SkillAnim.SetLabel(LevelText("Animais", AloneZUIState.StageAnim, AloneZUIState.MaxStageAnim, AloneZUIState.Anim, AloneZUIState.StepAnim));

        if (AloneZUIState.StageTime >= AloneZUIState.MaxStageTime)
        { m_SkillTime.UpdateBar(1, 1); }
        else
        { m_SkillTime.UpdateBar(AloneZUIState.TimeMinutes, AloneZUIState.StepTime); }
        m_SkillTime.SetLabel(LevelText("Tempo no servidor", AloneZUIState.StageTime, AloneZUIState.MaxStageTime, AloneZUIState.TimeMinutes, AloneZUIState.StepTime));

        if (AloneZUIState.StageDist >= AloneZUIState.MaxStageDist)
        { m_SkillDist.UpdateBar(1, 1); }
        else
        { m_SkillDist.UpdateBar(AloneZUIState.DistanceKm, AloneZUIState.StepDistance); }
        m_SkillDist.SetLabel(LevelText("Distancia", AloneZUIState.StageDist, AloneZUIState.MaxStageDist, AloneZUIState.DistanceKm, AloneZUIState.StepDistance));

        if (AloneZUIState.StageDeath >= AloneZUIState.MaxStageDeath)
        { m_SkillDeath.UpdateBar(1, 1); }
        else
        { m_SkillDeath.UpdateBar(AloneZUIState.Deaths, AloneZUIState.StepDeath); }
        m_SkillDeath.SetLabel(LevelText("Players mortos", AloneZUIState.StageDeath, AloneZUIState.MaxStageDeath, AloneZUIState.Deaths, AloneZUIState.StepDeath));

        if (m_RewardCounter) m_RewardCounter.SetText("Recompensas ganhas: " + AloneZUIState.RewardCount.ToString());
    }

    void SetAlpha(float a)
    {
        if (m_SkillZed)  m_SkillZed.SetAlpha(a);
        if (m_SkillAnim) m_SkillAnim.SetAlpha(a);
        if (m_SkillTime) m_SkillTime.SetAlpha(a);
        if (m_SkillDist) m_SkillDist.SetAlpha(a);
        if (m_SkillDeath) m_SkillDeath.SetAlpha(a);
    }
}

class AloneZInvSkill
{
    void SetY(float y)
    {
        if (m_Main) m_Main.SetPos(0, y);
    }

    protected WrapSpacerWidget m_Parent;
    protected Widget m_Main;
    protected ProgressBarWidget m_Bar;
    protected TextWidget m_Label;

    void AloneZInvSkill(out WrapSpacerWidget w)
    {
        Widget root = GetGame().GetWorkspace().CreateWidgets("AloneZ_Progress/gui/layouts/alonez/alonez_inv_bar.layout", w);
        m_Parent = w;
        m_Main   = root;
        m_Bar    = ProgressBarWidget.Cast(root.FindAnyWidget("AloneZBar"));
        m_Label  = TextWidget.Cast(root.FindAnyWidget("AloneZLabel"));
    }

    void SetLabel(string t) { if (m_Label) m_Label.SetText(t); }
    void SetColor(int c)   { if (m_Bar)   m_Bar.SetColor(c); }

    void UpdateBar(int val, int maxv)
    {
        if (maxv <= 0) maxv = 1;
        float norm = val / (float)maxv;
        if (norm < 0.0) norm = 0.0;
        if (norm > 1.0) norm = 1.0;
        int percent = Math.Round(norm * 100);
        if (m_Bar) m_Bar.SetCurrent(percent);
    }

    void SetAlpha(float a)
    {
        if (m_Main)  m_Main.SetAlpha(a);
        if (m_Bar)   m_Bar.SetAlpha(a);
        if (m_Label) m_Label.SetAlpha(a);
    }
}



