//-----------------------------------------------------------------------------
/*!
   \file
   \brief       CAN monitor utility functions (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     05.02.2019  STW/G.Landsgesell
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMUTI_H
#define C_CAMUTI_H

/* -- Includes ------------------------------------------------------------- */
#include <QString>
#include <QWidget>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamUti
{
public:
   static QString h_GetAbsPathFromProj(const QString & orc_Path);
   static QString h_AskUserToSaveRelativePath(QWidget * const opc_Parent, const QString & orc_Path,
                                              const QString & orc_AbsoluteReferenceDir);

private:
   C_CamUti(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
