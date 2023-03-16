//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Parameter table model (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITPATREEDELEGATE_H
#define C_SYVDAITPATREEDELEGATE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QStyledItemDelegate>
#include "stwtypes.hpp"
#include "C_OscNodeDataPoolListElementId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaItPaTreeDelegate :
   public QStyledItemDelegate
{
   Q_OBJECT

public:
   C_SyvDaItPaTreeDelegate(QObject * const opc_Parent = NULL);

   QWidget * GetEditor(void);

   QWidget * createEditor(QWidget * const opc_Parent, const QStyleOptionViewItem & orc_Option,
                          const QModelIndex & orc_Index) const override;
   void setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const override;
   void setModelData(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                     const QModelIndex & orc_Index) const override;
   QSize sizeHint(const QStyleOptionViewItem & orc_Option, const QModelIndex & orc_Index) const override;
   void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
              const QModelIndex & orc_Index) const override;
   void destroyEditor(QWidget * const opc_Editor, const QModelIndex & orc_Index) const override;
   static int32_t h_GetTableItemHeight();

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigNewEditor(QWidget * const opc_Editor) const;
   void SigLinkClicked(const QModelIndex & orc_Index) const;
   void SigActionRead(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                      const uint32_t ou32_ValidLayers) const;
   void SigActionWrite(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                       const uint32_t ou32_ValidLayers) const;
   void SigActionApply(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                       const uint32_t ou32_ValidLayers) const;
   void SigActionLoad(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                      const uint32_t ou32_ValidLayers) const;
   void SigActionSave(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                      const uint32_t ou32_ValidLayers) const;
   void SigActionRecord(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                        const uint32_t ou32_ValidLayers) const;
   void SigActionRemove(const stw::opensyde_core::C_OscNodeDataPoolListElementId & orc_Id,
                        const uint32_t ou32_ValidLayers) const;

protected:
   bool eventFilter(QObject * const opc_Object, QEvent * const opc_Event) override;

private:
   void m_SetNewEditor(QWidget * const opc_Editor);
   QWidget * m_CreateEditor(QWidget * const opc_Parent, const QModelIndex & orc_Index) const;

   QWidget * mpc_Editor;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
