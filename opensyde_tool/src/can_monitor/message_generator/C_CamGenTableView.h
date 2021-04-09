//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View (display) component for message generator table (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMGENTABLEVIEW_H
#define C_CAMGENTABLEVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QSortFilterProxyModel>
#include "C_OgePubIconOnly.h"
#include "C_OgeContextMenu.h"
#include "C_CamGenTableModel.h"
#include "C_TblViewInteraction.h"
#include "C_CamGenTableDelegate.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamGenTableView :
   public C_TblViewInteraction
{
   Q_OBJECT

public:
   C_CamGenTableView(QWidget * const opc_Parent = NULL);
   ~C_CamGenTableView(void);

   void AddMessageFromDatabase(void);
   void AddMessage(void);
   void CopyMessage(void) const;
   void PasteMessage(void);
   void CutMessage(void);
   void DeleteMessage(void);
   void MoveMessageUp(void);
   void MoveMessageDown(void);

   void SetCurrentColumnWidths(const std::vector<stw_types::sint32> & orc_ColumnWidths);
   std::vector<stw_types::sint32> GetCurrentColumnWidths(void) const;

   void SaveUserSettings(void) const;
   void LoadUserSettings(void);

   void RemoveMessagesForFile(const QString & orc_File, const std::vector<stw_types::uint32> * const opc_Indices);
   void SetCommunicationStarted(const bool oq_Active);
   void SetCyclicActive(const bool oq_Active);
   void UpdateMessageData(const stw_types::uint32 ou32_MessageIndex);
   void TriggerModelUpdateCyclicMessage(const stw_types::uint32 ou32_MessageIndex, const bool oq_Active);
   bool CheckAndHandleKey(const QString & orc_Input);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigItemCountChanged(const stw_types::uint32 ou32_NewItemCount);
   void SigUpdateMessageDLC(const stw_types::uint32 ou32_MessageIndex);
   void SigRegisterCyclicMessage(const stw_types::uint32 ou32_MessageIndex, const bool oq_Active);
   void SigSendMessage(const stw_types::uint32 ou32_MessageIndex, const stw_types::uint32 ou32_TimeToSend);
   void SigSelected(const stw_types::uint32 ou32_NumSelectedItems, const stw_types::uint32 ou32_Row);

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   virtual void showEvent(QShowEvent * const opc_Event) override;
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   virtual void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;
   virtual stw_types::sintn sizeHintForColumn(stw_types::sintn osn_Column) const override;

private:
   bool mq_CommunicationActive; ///< True if message trace is currently running
   bool mq_CyclicTransmissionActive;
   C_CamGenTableDelegate mc_Delegate;
   QSortFilterProxyModel mc_SortProxyModel;
   stw_opensyde_gui_logic::C_CamGenTableModel mc_Model;
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   QAction * mpc_ActionAddFromDatabase;
   QAction * mpc_ActionAdd;
   QAction * mpc_ActionCut;
   QAction * mpc_ActionCopy;
   QAction * mpc_ActionPaste;
   QAction * mpc_ActionDelete;
   stw_opensyde_gui_elements::C_OgePubIconOnly * mpc_PushButtonScrollTop;

   std::vector<stw_types::uint32> m_GetSelectedRows(void) const;
   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_SelectRange(const std::vector<stw_types::uint32> & orc_Items);
   void m_SelectAndScrollToRow(const stw_types::uint32 ou32_Row);
   void m_RepositionButtons(void);
   void m_HandleLinkClicked(const QModelIndex & orc_Index);
   void m_StopCyclicCommunication(const std::vector<stw_types::uint32> & orc_Items);
   void m_ModelRegisterCyclicMessage(const stw_types::uint32 ou32_MessageIndex, const bool oq_Active);
   void m_AddMessageFromDatabase(const std::vector<std::array<QString, 2> > & orc_NewItems);
   void m_HandleNewItemScrollingAndSelection(const std::vector<stw_types::uint32> & orc_Indices);
   static std::vector<stw_types::sint32> mh_ConvertVector(const std::vector<stw_types::uint32> & orc_Input);
   void m_SetSelectedMessages(const std::vector<stw_types::sint32> & orc_SelectedIndices);
   void m_SelectRow(const stw_types::sint32 os32_Row, const QItemSelectionModel::SelectionFlags & orc_Flags);
   void m_HandleCheckChange(const QModelIndex & orc_Index);
   void m_RegisterAllCyclicMessages(void);
   void m_Report(const stw_opensyde_gui_elements::C_OgeWiCustomMessage::E_Type oe_Type, const QString & orc_Heading,
                 const QString & orc_Description);

   //Avoid call
   C_CamGenTableView(const C_CamGenTableView &);
   C_CamGenTableView & operator =(const C_CamGenTableView &);

   const static stw_types::sintn mhsn_COL_WIDTH_DATA;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
