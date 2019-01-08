//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for changeable mouse cursor on hover over (header)

   Base class for changeable mouse cursor on hover over (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GIBICUSTOMMOUSEITEM_H
#define C_GIBICUSTOMMOUSEITEM_H

/* -- Includes ------------------------------------------------------------- */

#include <QCursor>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_GiBiCustomMouseItem
{
public:
   C_GiBiCustomMouseItem(void);
   virtual ~C_GiBiCustomMouseItem(void);
   virtual void RestoreDefaultCursor(void) = 0;
   virtual void SetTemporaryCursor(const QCursor & orc_TemporaryCursor) = 0;
   virtual void SetDefaultCursor(const QCursor & orc_Value) = 0;

protected:
   QCursor mc_DefaultCursor; ///< Default cursor (Used in most situations, e.g. nothing special is happening)
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
