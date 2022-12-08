//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Model component for message generator table (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMGENTABLEMODEL_H
#define C_CAMGENTABLEMODEL_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_TblModelAction.hpp"
#include "C_CamProMessageData.hpp"
#include "C_OgeWiCustomMessage.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamGenTableModel :
   public C_TblModelAction
{
   Q_OBJECT

public:
   enum E_Columns
   {
      eNAME = 0,
      eXTD,
      eRTR,
      eID,
      eDLC,
      eDATA,
      eCYCLIC_TRIGGER,
      eCYCLIC_TIME,
      eKEY,
      eMANUAL_TRIGGER
   };

   C_CamGenTableModel(QObject * const opc_Parent = NULL);

   void UpdateMessageKey(const uint32_t ou32_MessageIndex);
   void UpdateMessageData(const uint32_t ou32_MessageIndex);
   void TriggerModelUpdateCyclicMessage(const uint32_t ou32_MessageIndex, const bool oq_Active);

   QVariant headerData(const int32_t os32_Section, const Qt::Orientation oe_Orientation,
                       const int32_t os32_Role = static_cast<int32_t>(Qt::DisplayRole))
   const override;
   int32_t columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   QVariant data(const QModelIndex & orc_Index, const int32_t os32_Role =
                    static_cast<int32_t>(Qt::DisplayRole)) const override;
   bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                const int32_t os32_Role = static_cast<int32_t>(Qt::EditRole)) override;
   Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

   void CopySelectedItems(const std::vector<uint32_t> & orc_SelectedIndices) const override;

   static E_Columns h_ColumnToEnum(const int32_t os32_Column);
   static int32_t h_EnumToColumn(const E_Columns oe_Value);

   std::vector<uint32_t> AddSpecificNewItems(const std::vector<uint32_t> & orc_SelectedIndex,
                                             const std::vector<C_CamProMessageData> & orc_Messages);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigItemCheck(const QModelIndex & orc_Index);
   void SigReport(const stw::opensyde_gui_elements::C_OgeWiCustomMessage::E_Type oe_Type, const QString & orc_Heading,
                  const QString & orc_Description);
   void SigUpdateMessageDlc(const uint32_t ou32_MessageIndex);
   void SigRegisterCyclicMessage(const uint32_t ou32_MessageIndex, const bool oq_Active);

protected:
   uint32_t m_AddNewItem(const uint32_t ou32_SelectedIndex) override;
   std::vector<uint32_t> m_PasteItems(const uint32_t ou32_SelectedIndex) override;
   uint32_t m_GetSizeItems(void) const override;
   void m_DeleteItem(const uint32_t ou32_Index) override;
   void m_MoveItem(const uint32_t ou32_SourceIndex, const uint32_t ou32_TargetIndex) override;

private:
   std::vector<uint32_t> m_AddNewMessages(const uint32_t ou32_SelectedIndex,
                                          const std::vector<C_CamProMessageData> & orc_Data);

   void m_CheckAndHandleRegisterCyclicMessage(const uint32_t ou32_MessageIndex, const bool oq_Active);
   void m_SpecialXtdFlagSetHandling(const int32_t os32_Row, const uint32_t ou32_Index, const int32_t os32_Role);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
