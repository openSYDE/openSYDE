//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for view item in navigation bar (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_NAGVIEWITEM_H
#define C_NAGVIEWITEM_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QIcon>
#include <QWidget>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_NagViewItem;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagViewItem :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_NagViewItem(QWidget * const opc_Parent = NULL);
   ~C_NagViewItem(void) override;

   void LoadUserSettings(void);
   void SaveUserSettings(void) const;
   void UpdateName(void) const;
   void UpdateDeco(void) const;
   void InitStaticNames(void) const;
   void SetDragged(const bool oq_Active);
   void Init(const uint32_t ou32_ViewIndex);
   void SetActive(const bool oq_Active, const int32_t os32_SubMode);
   void SetHovered(const bool oq_Hovered) const;
   void HandleServiceMode(void);

   QSize sizeHint(void) const override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigStartDrag(const C_NagViewItem * const opc_Sender);
   void SigDuplicate(const C_NagViewItem * const opc_Sender);
   void SigDelete(const C_NagViewItem * const opc_Sender);
   void SigExpanded(const C_NagViewItem * const opc_Sender);
   void SigSetName(const C_NagViewItem * const opc_Sender, const QString & orc_Name);
   void SigSelect(const C_NagViewItem * const opc_Sender, const int32_t os32_SubMode, const QString & orc_Name,
                  const QString & orc_SubSubMode);

protected:
   bool event(QEvent * const opc_Event) override;
   void paintEvent(QPaintEvent * const opc_Event) override;

private:
   Ui::C_NagViewItem * mpc_Ui;

   bool mq_Active;
   bool mq_IgnoreActiveOnExpand;
   uint32_t mu32_ViewIndex;
   static const int32_t mhs32_SIZE_TOP;
   static const int32_t mhs32_SIZE_SUB;
   static const int32_t mhs32_FIX_SIZE_BOTTOM;
   bool mq_ButtonPressed;
   QMetaObject::Connection mc_Conn;
   const QIcon mc_IconBulletList;
   const QIcon mc_IconBulletListActive;

   //Avoid call
   C_NagViewItem(const C_NagViewItem &);
   C_NagViewItem & operator =(const C_NagViewItem &) &;

   bool m_IsExpanded(void) const;
   void m_OnNameEditFinished(void);
   void m_OnNameEditCancelled(void);
   void m_SetExpanded(const bool oq_Expanded);
   void m_HandleExpandIcon(const bool oq_Expanded);
   void m_HandleSubItemNames(void);
   void m_OnExpand(void);
   void m_OnDelete(void);
   void m_OnStartDrag(void);
   void m_OnDuplicate(void);
   void m_OnEditButton(void) const;
   void m_OnSetupClicked(void);
   void m_OnUpdateClicked(void);
   void m_OnDashboardClicked(void);
   void m_HandleServiceModeColors(void);

   void m_ButtonPressed(void);
   void m_ButtonReleased(void);
   static QIcon mh_PrepareIcon(const QString & orc_Path);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
