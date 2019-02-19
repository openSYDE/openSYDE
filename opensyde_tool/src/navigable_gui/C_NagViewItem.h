//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for view item in navigation bar (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.07.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_NAGVIEWITEM_H
#define C_NAGVIEWITEM_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_NagViewItem;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_NagViewItem :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_NagViewItem(QWidget * const opc_Parent = NULL);
   ~C_NagViewItem(void);

   void LoadUserSettings(void);
   void SaveUserSettings(void) const;
   void UpdateName(void) const;
   void UpdateDeco(void) const;
   void InitStaticNames(void) const;
   void SetDragged(const bool oq_Active);
   void Init(const stw_types::uint32 ou32_ViewIndex);
   void SetActive(const bool oq_Active, const stw_types::sint32 os32_SubMode);
   void SetHovered(const bool oq_Hovered) const;

   virtual QSize sizeHint(void) const override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigStartDrag(const C_NagViewItem * const opc_Sender);
   void SigDuplicate(const C_NagViewItem * const opc_Sender);
   void SigDelete(const C_NagViewItem * const opc_Sender);
   void SigExpanded(const C_NagViewItem * const opc_Sender);
   void SigSetName(const C_NagViewItem * const opc_Sender, const QString & orc_Name);
   void SigSelect(const C_NagViewItem * const opc_Sender, const stw_types::sint32 os32_SubMode,
                  const QString & orc_Name, const QString & orc_SubSubMode);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual bool event(QEvent * const opc_Event) override;
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore

private:
   Ui::C_NagViewItem * mpc_Ui;

   bool mq_Active;
   bool mq_IgnoreActiveOnExpand;
   stw_types::uint32 mu32_ViewIndex;
   static const stw_types::sintn hsn_SizeTop;
   static const stw_types::sintn hsn_SizeSub;
   static const stw_types::sintn hsn_FixSizeBottom;
   bool mq_ButtonPressed;

   //Avoid call
   C_NagViewItem(const C_NagViewItem &);
   C_NagViewItem & operator =(const C_NagViewItem &);

   bool m_IsExpanded(void) const;
   void m_OnNameEditFinished(void);
   void m_OnNameEditCancelled(void);
   void m_SetExpanded(const bool oq_Expanded);
   void m_OnExpand(void);
   void m_OnDelete(void);
   void m_OnStartDrag(void);
   void m_OnDuplicate(void);
   void m_OnEditButton(void) const;
   void m_OnSetupClicked(void);
   void m_OnUpdateClicked(void);
   void m_OnDashboardClicked(void);

   void m_ButtonPressed(void);
   void m_ButtonReleased(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
