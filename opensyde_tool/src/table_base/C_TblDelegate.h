//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Delegate (interaction) component base for any table (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_TBLDELEGATE_H
#define C_TBLDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QStyledItemDelegate>

#include "constants.h"
#include "C_OgeCbxTableBase.h"
#include "C_OgeCbxMultiSelect.h"
#include "C_OgeWiSpinBoxGroup.h"
#include "C_TblEditLineEditBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblDelegate :
   public QStyledItemDelegate
{
   Q_OBJECT

public:
   C_TblDelegate(QObject * const opc_Parent = NULL);

   virtual QWidget * createEditor(QWidget * const opc_Parent, const QStyleOptionViewItem & orc_Option,
                                  const QModelIndex & orc_Index) const override;
   virtual void setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const override;
   virtual void setModelData(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                             const QModelIndex & orc_Index) const override;
   virtual void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                      const QModelIndex & orc_Index) const;

protected:
   //----------------------------------------------------------------------------------------------------------------------
   /*! \brief  Create combo box

      \param[in,out]  opc_Parent    Parent

      \return
      Return correctly styled combo box
   */
   //----------------------------------------------------------------------------------------------------------------------
   virtual stw_opensyde_gui_elements::C_OgeCbxTableBase * m_CreateComboBox(QWidget * const opc_Parent) const = 0;
   //----------------------------------------------------------------------------------------------------------------------
   /*! \brief  Create multi select combo box

      \param[in,out]  opc_Parent    Parent

      \return
      Return correctly styled multi select combo box
   */
   //----------------------------------------------------------------------------------------------------------------------
   virtual stw_opensyde_gui_elements::C_OgeCbxMultiSelect * m_CreateMultiSelectComboBox(QWidget * const opc_Parent)
   const = 0;
   //----------------------------------------------------------------------------------------------------------------------
   /*! \brief  Create line edit

      \param[in,out]  opc_Parent    Parent

      \return
      Return correctly styled line edit
   */
   //----------------------------------------------------------------------------------------------------------------------
   virtual stw_opensyde_gui::C_TblEditLineEditBase * m_CreateLineEdit(QWidget * const opc_Parent) const = 0;
   //----------------------------------------------------------------------------------------------------------------------
   /*! \brief  Create spin box

      \param[in,out]  opc_Parent    Parent

      \return
      Return correctly styled spin box
   */
   //----------------------------------------------------------------------------------------------------------------------
   virtual stw_opensyde_gui_elements::C_OgeWiSpinBoxGroup * m_CreateSpinBox(QWidget * const opc_Parent) const = 0;

   bool mq_InitialSelection;

private:
   static E_UserRoleInteractionElementValue mh_GetInteractionElementValue(const QModelIndex & orc_Index);
   QWidget * m_CreateGenericEditor(QWidget * const opc_Parent, const QModelIndex & orc_Index,
                                   const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Min,
                                   const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Max,
                                   const stw_types::float64 of64_Factor, const stw_types::float64 of64_Offset,
                                   const stw_types::uint32 & oru32_ArrayIndex) const;
   void m_SetGenericEditorDataVariable(QWidget * const opc_Editor, const QModelIndex & orc_Index) const;
   void m_CommitData(void);
   static void mh_SetModelGenericDataVariable(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                                              const QModelIndex & orc_Index);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
