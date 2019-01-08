//-----------------------------------------------------------------------------
/*!
   \file
   \brief       System view dashboard data element update mode table delegate (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     08.09.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVDAPEUPDATEMODETABLEDELEGATE_H
#define C_SYVDAPEUPDATEMODETABLEDELEGATE_H

/* -- Includes ------------------------------------------------------------- */
#include <QStyledItemDelegate>
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvDaPeUpdateModeTableDelegate :
   public QStyledItemDelegate
{
public:
   C_SyvDaPeUpdateModeTableDelegate(const stw_types::uint32 ou32_ViewIndex);

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual QWidget * createEditor(QWidget * const opc_Parent, const QStyleOptionViewItem & orc_Option,
                                  const QModelIndex & orc_Index) const override;
   virtual void setEditorData(QWidget * const opc_Editor, const QModelIndex & orc_Index) const override;
   virtual void setModelData(QWidget * const opc_Editor, QAbstractItemModel * const opc_Model,
                             const QModelIndex & orc_Index) const override;
   //lint -restore

private:
   const stw_types::uint32 mu32_ViewIndex;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
