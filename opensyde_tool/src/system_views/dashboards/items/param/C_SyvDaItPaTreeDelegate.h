//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Parameter table model (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     03.11.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVDAITPATREEDELEGATE_H
#define C_SYVDAITPATREEDELEGATE_H

/* -- Includes ------------------------------------------------------------- */
#include <QStyledItemDelegate>
#include "stwtypes.h"
#include "C_OSCNodeDataPoolListElementId.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvDaItPaTreeDelegate :
   public QStyledItemDelegate
{
   Q_OBJECT

public:
   C_SyvDaItPaTreeDelegate(QObject * const opc_Parent = NULL);

   QWidget * GetEditor(void);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual QWidget * createEditor(QWidget * const opc_Parent, const QStyleOptionViewItem & orc_Option,
                                  const QModelIndex & orc_Index) const override;
   virtual void setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const override;
   virtual void setModelData(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                             const QModelIndex & orc_Index) const override;
   virtual QSize sizeHint(const QStyleOptionViewItem & orc_Option, const QModelIndex & orc_Index) const override;
   virtual void paint(QPainter * const opc_Painter, const QStyleOptionViewItem & orc_Option,
                      const QModelIndex & orc_Index) const override;
   virtual void destroyEditor(QWidget * const opc_Editor, const QModelIndex & orc_Index) const override;
   //lint -restore

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigNewEditor(QWidget * const opc_Editor) const;
   void SigLinkClicked(const QModelIndex & orc_Index) const;
   void SigActionRead(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                      const stw_types::uint32 ou32_ValidLayers) const;
   void SigActionWrite(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                       const stw_types::uint32 ou32_ValidLayers) const;
   void SigActionApply(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                       const stw_types::uint32 ou32_ValidLayers) const;
   void SigActionLoad(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                      const stw_types::uint32 ou32_ValidLayers) const;
   void SigActionSave(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                      const stw_types::uint32 ou32_ValidLayers) const;
   void SigActionRecord(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                        const stw_types::uint32 ou32_ValidLayers) const;
   void SigActionRemove(const stw_opensyde_core::C_OSCNodeDataPoolListElementId & orc_Id,
                        const stw_types::uint32 ou32_ValidLayers) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual bool eventFilter(QObject * const opc_Object, QEvent * const opc_Event) override;
   //lint -restore

private:
   void m_SetNewEditor(QWidget * const opc_Editor);
   QWidget * m_CreateEditor(QWidget * const opc_Parent, const QModelIndex & orc_Index) const;

   QWidget * mpc_Editor;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
