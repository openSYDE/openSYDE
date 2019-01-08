//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Tree view item for C_OgeTreeViewNaviBar (header)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     09.03.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGETREEVIEWITEMNAVIBAR_H
#define C_OGETREEVIEWITEMNAVIBAR_H

/* -- Includes ------------------------------------------------------------- */

#include <QTreeWidgetItem>
#include <QString>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeTreeViewItemNaviBar :
   public QTreeWidgetItem
{
public:
   explicit C_OgeTreeViewItemNaviBar(QTreeWidgetItem * const opc_Parent = NULL);
   virtual ~C_OgeTreeViewItemNaviBar();

   void SetMainText(const QString & orc_Text);
   QString GetMainText(void) const;
   void SetChangeMark(const bool oq_Active);

private:
   void m_UpdateShownText(void);

   bool mq_ChangeMarkActive;
   QString mc_OriginalText;

   static const QString mhc_ChangeMark;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
