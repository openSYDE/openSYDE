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

#include "constants.hpp"
#include "C_OgeCbxTableBase.hpp"
#include "C_OgeCbxMultiSelect.hpp"
#include "C_OgeWiSpinBoxGroup.hpp"
#include "C_TblEditLineEditBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_TblDelegate :
   public QStyledItemDelegate
{
   Q_OBJECT

public:
   C_TblDelegate(QObject * const opc_Parent = NULL);

   QWidget * createEditor(QWidget * const opc_Parent, const QStyleOptionViewItem & orc_Option,
                          const QModelIndex & orc_Index) const override;
   void setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const override;
   void setModelData(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                     const QModelIndex & orc_Index) const override;
   void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
              const QModelIndex & orc_Index) const override;

protected:
   //----------------------------------------------------------------------------------------------------------------------
   /*! \brief  Create combo box

      \param[in,out]  opc_Parent    Parent

      \return
      Return correctly styled combo box
   */
   //----------------------------------------------------------------------------------------------------------------------
   virtual stw::opensyde_gui_elements::C_OgeCbxTableBase * m_CreateComboBox(QWidget * const opc_Parent) const = 0;
   //----------------------------------------------------------------------------------------------------------------------
   /*! \brief  Create multi select combo box

      \param[in,out]  opc_Parent    Parent

      \return
      Return correctly styled multi select combo box
   */
   //----------------------------------------------------------------------------------------------------------------------
   virtual stw::opensyde_gui_elements::C_OgeCbxMultiSelect * m_CreateMultiSelectComboBox(QWidget * const opc_Parent)
   const = 0;
   //----------------------------------------------------------------------------------------------------------------------
   /*! \brief  Create line edit

      \param[in,out]  opc_Parent    Parent

      \return
      Return correctly styled line edit
   */
   //----------------------------------------------------------------------------------------------------------------------
   virtual stw::opensyde_gui::C_TblEditLineEditBase * m_CreateLineEdit(QWidget * const opc_Parent) const = 0;
   //----------------------------------------------------------------------------------------------------------------------
   /*! \brief  Create spin box

      \param[in,out]  opc_Parent    Parent

      \return
      Return correctly styled spin box
   */
   //----------------------------------------------------------------------------------------------------------------------
   virtual stw::opensyde_gui_elements::C_OgeWiSpinBoxGroup * m_CreateSpinBox(QWidget * const opc_Parent) const = 0;

   bool mq_InitialSelection;

private:
   static E_UserRoleInteractionElementValue mh_GetInteractionElementValue(const QModelIndex & orc_Index);
   QWidget * m_CreateGenericEditor(QWidget * const opc_Parent, const QModelIndex & orc_Index,
                                   const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Min,
                                   const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Max,
                                   const float64_t of64_Factor, const float64_t of64_Offset,
                                   const uint32_t & oru32_ArrayIndex) const;
   void m_SetGenericEditorDataVariable(QWidget * const opc_Editor, const QModelIndex & orc_Index) const;
   void m_CommitData(void);
   static void mh_SetModelGenericDataVariable(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                                              const QModelIndex & orc_Index);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
