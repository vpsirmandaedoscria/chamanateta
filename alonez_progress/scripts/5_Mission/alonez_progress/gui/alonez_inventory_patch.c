modded class Inventory
{
    static ref AloneZInvSkillContainer s_AloneZCont;

    void Inventory(LayoutHolder parent)
    {
        Widget left = GetMainWidget().FindAnyWidget("LeftPanel");
        if (!left) return;
        Widget scroller = left.FindAnyWidget("Scroller");
        if (!scroller) return;
        Widget content = scroller.FindAnyWidget("Content");
        if (!content) return;

        if (!content.FindAnyWidget("AloneZInvRoot"))
        {
            s_AloneZCont = new AloneZInvSkillContainer(content);
            Print("[AloneZProgressUI] Inventory -> AloneZInvSkillContainer created (respawn or missing)");
        }
        else
        {
            if (!s_AloneZCont)
                s_AloneZCont = new AloneZInvSkillContainer(content);
        }
    }
}




