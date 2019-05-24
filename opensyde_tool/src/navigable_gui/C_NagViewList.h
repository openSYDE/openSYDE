//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List widget for view widgets (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_NAGVIEWLIST_H
#define C_NAGVIEWLIST_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_NagViewItem.h"
#include "C_NagViewListModel.h"
#include "C_NagViewListDelegate.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagViewList :
   public QListView
{
   Q_OBJECT

public:
   C_NagViewList(QWidget * const opc_Parent = NULL);
   ~C_NagViewList(void);

   void Init(void);
   void LoadUserSettings(void) const;
   void SaveUserSettings(void) const;
   void UpdateNames(void) const;
   void UpdateDeco(const bool oq_CheckOnlyThisView, const stw_types::uint32 ou32_ViewIndex) const;
   stw_types::sint32 GetMaximumRequiredHeight(void) const;
   void SetActive(const stw_types::uint32 ou32_ViewIndex, const stw_types::sint32 os32_SubMode) const;
   C_NagViewItem * GetItemAt(const stw_types::sintn osn_Index) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigSizeChange(void);
   void SigDuplicate(const stw_types::uint32 ou32_ViewIndex);
   void SigDelete(const stw_types::uint32 ou32_ViewIndex);
   void SigMoveView(const stw_types::uint32 ou32_SourceViewIndex, const stw_types::uint32 ou32_TargetViewIndex);
   void SigSetName(const stw_types::uint32 ou32_ViewIndex, const QString & orc_Name);
   void SigSelect(const stw_types::uint32 ou32_ViewIndex, const stw_types::sint32 os32_SubMode,
                  const QString & orc_Name, const QString & orc_SubSubItemName);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   virtual void dragMoveEvent(QDragMoveEvent * const opc_Event) override;
   virtual void dropEvent(QDropEvent * const opc_Event) override;
   virtual void startDrag(const Qt::DropActions oc_SupportedActions) override;
   //lint -restore

private:
   C_NagViewListModel mc_Model;
   C_NagViewListDelegate mc_Delegate;

   //Avoid call
   C_NagViewList(const C_NagViewList &);
   C_NagViewList & operator =(const C_NagViewList &);

   void m_AfterDrag(void) const;
   void m_StartDrag(const QModelIndex & orc_Index);
   void m_OnStartDrag(const C_NagViewItem * const opc_Sender);
   void m_OnExpand(const C_NagViewItem * const opc_Sender);
   void m_OnDelete(const C_NagViewItem * const opc_Sender);
   void m_OnDuplicate(const C_NagViewItem * const opc_Sender);
   void m_OnSetName(const C_NagViewItem * const opc_Sender, const QString & orc_Name);
   void m_OnSelect(const C_NagViewItem * const opc_Sender, const stw_types::sint32 os32_SubMode,
                   const QString & orc_Name, const QString & orc_SubSubItemName);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
