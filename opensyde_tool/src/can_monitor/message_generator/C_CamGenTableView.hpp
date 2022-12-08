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
#include "C_OgePubIconOnly.hpp"
#include "C_OgeContextMenu.hpp"
#include "C_CamGenTableModel.hpp"
#include "C_TblViewInteraction.hpp"
#include "C_CamGenTableDelegate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamGenTableView :
   public C_TblViewInteraction
{
   Q_OBJECT

public:
   C_CamGenTableView(QWidget * const opc_Parent = NULL);
   ~C_CamGenTableView(void) override;

   void AddMessageFromDatabase(void);
   void AddMessage(void);
   void CopyMessage(void) const;
   void PasteMessage(void);
   void CutMessage(void);
   void DeleteMessage(void);
   void MoveMessageUp(void);
   void MoveMessageDown(void);

   void SetCurrentColumnWidths(const std::vector<int32_t> & orc_ColumnWidths);
   std::vector<int32_t> GetCurrentColumnWidths(void) const;

   void SaveUserSettings(void) const;
   void LoadUserSettings(void);

   void RemoveMessagesForFile(const QString & orc_File, const std::vector<uint32_t> * const opc_Indices);
   void SetCommunicationStarted(const bool oq_Active);
   void SetCyclicActive(const bool oq_Active);
   void UpdateMessageData(const uint32_t ou32_MessageIndex);
   void TriggerModelUpdateCyclicMessage(const uint32_t ou32_MessageIndex, const bool oq_Active);
   bool CheckAndHandleKey(const QString & orc_Input);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigItemCountChanged(const uint32_t ou32_NewItemCount);
   void SigUpdateMessageDlc(const uint32_t ou32_MessageIndex);
   void SigRegisterCyclicMessage(const uint32_t ou32_MessageIndex, const bool oq_Active);
   void SigSendMessage(const uint32_t ou32_MessageIndex, const uint32_t ou32_TimeToSend);
   void SigSelected(const uint32_t ou32_NumSelectedItems, const uint32_t ou32_Row);

protected:
   void keyPressEvent(QKeyEvent * const opc_Event) override;
   void showEvent(QShowEvent * const opc_Event) override;
   void resizeEvent(QResizeEvent * const opc_Event) override;
   void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;
   int32_t sizeHintForColumn(const int32_t os32_Column) const override;

private:
   bool mq_CommunicationActive; ///< True if message trace is currently running
   bool mq_CyclicTransmissionActive;
   C_CamGenTableDelegate mc_Delegate;
   QSortFilterProxyModel mc_SortProxyModel;
   stw::opensyde_gui_logic::C_CamGenTableModel mc_Model;
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   QAction * mpc_ActionAddFromDatabase;
   QAction * mpc_ActionAdd;
   QAction * mpc_ActionCut;
   QAction * mpc_ActionCopy;
   QAction * mpc_ActionPaste;
   QAction * mpc_ActionDelete;
   stw::opensyde_gui_elements::C_OgePubIconOnly * mpc_PushButtonScrollTop;

   std::vector<uint32_t> m_GetSelectedRows(void) const;
   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_SelectRange(const std::vector<uint32_t> & orc_Items);
   void m_SelectAndScrollToRow(const uint32_t ou32_Row);
   void m_RepositionButtons(void);
   void m_HandleLinkClicked(const QModelIndex & orc_Index);
   void m_StopCyclicCommunication(const std::vector<uint32_t> & orc_Items);
   void m_ModelRegisterCyclicMessage(const uint32_t ou32_MessageIndex, const bool oq_Active);
   void m_AddMessageFromDatabase(const std::vector<std::array<QString, 2> > & orc_NewItems);
   void m_HandleNewItemScrollingAndSelection(const std::vector<uint32_t> & orc_Indices);
   static std::vector<int32_t> mh_ConvertVector(const std::vector<uint32_t> & orc_Input);
   void m_SetSelectedMessages(const std::vector<int32_t> & orc_SelectedIndices);
   void m_SelectRow(const int32_t os32_Row, const QItemSelectionModel::SelectionFlags & orc_Flags);
   void m_HandleCheckChange(const QModelIndex & orc_Index);
   void m_RegisterAllCyclicMessages(void);
   void m_Report(const stw::opensyde_gui_elements::C_OgeWiCustomMessage::E_Type oe_Type, const QString & orc_Heading,
                 const QString & orc_Description);

   //Avoid call
   C_CamGenTableView(const C_CamGenTableView &);
   C_CamGenTableView & operator =(const C_CamGenTableView &) &;

   const static int32_t mhs32_COL_WIDTH_DATA;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
