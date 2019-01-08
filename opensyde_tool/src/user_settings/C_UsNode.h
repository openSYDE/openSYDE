//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Project node user settings (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.01.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_USNODE_H
#define C_USNODE_H

/* -- Includes ------------------------------------------------------------- */
#include <QString>
#include <QMap>
#include "C_UsNodeDatapool.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_UsNode
{
public:
   C_UsNode(void);

   C_UsNodeDatapool GetDatapool(const QString & orc_DatapoolName) const;
   const QList<QString> GetDatapoolKeysInternal(void) const;

   QString GetSelectedDatapoolName(void) const;
   void SetSelectedDatapoolName(const QString & orc_DatapoolName);
   void SetDatapoolOpenListNames(const QString & orc_DatapoolName, const std::vector<QString> & orc_New);
   void SetDatapoolSelectedListNames(const QString & orc_DatapoolName, const std::vector<QString> & orc_New);
   void SetDatapoolSelectedVariableNames(const QString & orc_DatapoolName, const std::vector<QString> & orc_New);
   void SetDatapoolListSelectedMessage(const QString & orc_DatapoolName, const QString & orc_ListName,
                                       const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_SelectedProtocol,
                                       const bool oq_MessageSelected, const QString & orc_SelectedMessageName,
                                       const bool oq_SignalSelected, const QString & orc_SelectedSignalName);
   void SetDatapoolListColumnSizes(const QString & orc_DatapoolName, const QString & orc_ListName,
                                   const std::vector<stw_types::sint32> & orc_ColumnWidths);

private:
   QString mc_SelectedDataPoolName;
   QMap<QString, C_UsNodeDatapool> mc_Datapools; ///< History of last known node datapool user settings
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
