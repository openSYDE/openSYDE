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
#include "stwtypes.hpp"
#include "C_NagViewItem.hpp"
#include "C_NagViewListModel.hpp"
#include "C_NagViewListDelegate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagViewList :
   public QListView
{
   Q_OBJECT

public:
   C_NagViewList(QWidget * const opc_Parent = NULL);
   virtual ~C_NagViewList(void);

   void Init(void);
   void LoadUserSettings(void) const;
   void SaveUserSettings(void) const;
   void UpdateNames(void) const;
   void UpdateDeco(const bool oq_CheckOnlyThisView, const uint32_t ou32_ViewIndex) const;
   int32_t GetMaximumRequiredHeight(void) const;
   void SetActive(const uint32_t ou32_ViewIndex, const int32_t os32_SubMode) const;
   C_NagViewItem * GetItemAt(const int32_t os32_Index) const;
   void HandleServiceMode(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigSizeChange(void);
   void SigDuplicate(const uint32_t ou32_ViewIndex);
   void SigDelete(const uint32_t ou32_ViewIndex);
   void SigMoveView(const uint32_t ou32_SourceViewIndex, const uint32_t ou32_TargetViewIndex);
   void SigSetName(const uint32_t ou32_ViewIndex, const QString & orc_Name);
   void SigSelect(const uint32_t ou32_ViewIndex, const int32_t os32_SubMode, const QString & orc_Name,
                  const QString & orc_SubSubItemName);

protected:
   void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   void dragMoveEvent(QDragMoveEvent * const opc_Event) override;
   void dropEvent(QDropEvent * const opc_Event) override;
   void startDrag(const Qt::DropActions oc_SupportedActions) override;

private:
   C_NagViewListModel mc_Model;
   C_NagViewListDelegate mc_Delegate;

   //Avoid call
   C_NagViewList(const C_NagViewList &);
   C_NagViewList & operator =(const C_NagViewList &) &;

   void m_AfterDrag(void) const;
   void m_StartDrag(const QModelIndex & orc_Index);
   void m_OnStartDrag(const C_NagViewItem * const opc_Sender);
   void m_OnExpand(const C_NagViewItem * const opc_Sender);
   void m_OnDelete(const C_NagViewItem * const opc_Sender);
   void m_OnDuplicate(const C_NagViewItem * const opc_Sender);
   void m_OnSetName(const C_NagViewItem * const opc_Sender, const QString & orc_Name);
   void m_OnSelect(const C_NagViewItem * const opc_Sender, const int32_t os32_SubMode, const QString & orc_Name,
                   const QString & orc_SubSubItemName);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
