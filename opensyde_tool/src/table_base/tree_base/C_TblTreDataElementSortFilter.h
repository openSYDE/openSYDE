//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Filter for data elements tree model (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     07.09.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVDAPEDATAELEMENTTREESORTFILTER_H
#define C_SYVDAPEDATAELEMENTTREESORTFILTER_H

/* -- Includes ------------------------------------------------------------- */
#include <QSortFilterProxyModel>
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_TblTreDataElementSortFilter :
   public QSortFilterProxyModel
{
public:
   C_TblTreDataElementSortFilter(QObject * const opc_Parent = NULL);

   void SetFilter(const QString & orc_Text);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual bool filterAcceptsRow(const stw_types::sintn osn_SourceRow,
                                 const QModelIndex & orc_SourceParent) const override;
   //lint -restore
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
