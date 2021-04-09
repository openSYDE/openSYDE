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
#include "stwtypes.h"
#include "C_TblModelAction.h"
#include "C_CamProMessageData.h"
#include "C_OgeWiCustomMessage.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
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

   void UpdateMessageKey(const stw_types::uint32 ou32_MessageIndex);
   void UpdateMessageData(const stw_types::uint32 ou32_MessageIndex);
   void TriggerModelUpdateCyclicMessage(const stw_types::uint32 ou32_MessageIndex, const bool oq_Active);

   virtual QVariant headerData(const stw_types::sintn osn_Section, const Qt::Orientation oe_Orientation,
                               const stw_types::sintn osn_Role = static_cast<stw_types::sintn>(Qt::DisplayRole))
   const override;
   virtual stw_types::sintn columnCount(const QModelIndex & orc_Parent = QModelIndex()) const override;
   virtual QVariant data(const QModelIndex & orc_Index, const stw_types::sintn osn_Role =
                            static_cast<stw_types::sintn>(Qt::DisplayRole)) const override;
   virtual bool setData(const QModelIndex & orc_Index, const QVariant & orc_Value,
                        const stw_types::sintn osn_Role = static_cast<stw_types::sintn>(Qt::EditRole)) override;
   virtual Qt::ItemFlags flags(const QModelIndex & orc_Index) const override;

   virtual void CopySelectedItems(const std::vector<stw_types::uint32> & orc_SelectedIndices) const override;

   static E_Columns h_ColumnToEnum(const stw_types::sint32 os32_Column);
   static stw_types::sint32 h_EnumToColumn(const E_Columns oe_Value);

   std::vector<stw_types::uint32> AddSpecificNewItems(const std::vector<stw_types::uint32> & orc_SelectedIndex,
                                                      const std::vector<C_CamProMessageData> & orc_Messages);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigItemCheck(const QModelIndex & orc_Index);
   void SigReport(const stw_opensyde_gui_elements::C_OgeWiCustomMessage::E_Type oe_Type, const QString & orc_Heading,
                  const QString & orc_Description);
   void SigUpdateMessageDLC(const stw_types::uint32 ou32_MessageIndex);
   void SigRegisterCyclicMessage(const stw_types::uint32 ou32_MessageIndex, const bool oq_Active);

protected:
   virtual stw_types::uint32 m_AddNewItem(const stw_types::uint32 ou32_SelectedIndex) override;
   virtual std::vector<stw_types::uint32> m_PasteItems(const stw_types::uint32 ou32_SelectedIndex) override;
   virtual stw_types::uint32 m_GetSizeItems(void) const override;
   virtual void m_DeleteItem(const stw_types::uint32 ou32_Index) override;
   virtual void m_MoveItem(const stw_types::uint32 ou32_SourceIndex, const stw_types::uint32 ou32_TargetIndex) override;

private:
   std::vector<stw_types::uint32> m_AddNewMessages(const stw_types::uint32 ou32_SelectedIndex,
                                                   const std::vector<C_CamProMessageData> & orc_Data);

   void m_CheckAndHandleRegisterCyclicMessage(const stw_types::uint32 ou32_MessageIndex, const bool oq_Active);
   void m_SpecialXtdFlagSetHandling(const stw_types::sintn osn_Row, const stw_types::uint32 ou32_Index,
                                    const stw_types::sintn osn_Role);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
