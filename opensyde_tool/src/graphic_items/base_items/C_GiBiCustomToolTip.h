//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for changeable tool tip on hover over (header)

   Base class for changeable tool tip on hover over (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_GIBICUSTOMTOOLTIP_H
#define C_GIBICUSTOMTOOLTIP_H

/* -- Includes ------------------------------------------------------------- */

#include <QString>
#include "C_NagToolTip.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_GiBiCustomToolTip
{
public:
   C_GiBiCustomToolTip();
   virtual ~C_GiBiCustomToolTip();
   void RestoreDefaultToolTip(void);
   void SetDefaultToolTipHeading(const QString & orc_Text);
   void SetDefaultToolTipContent(const QString & orc_Text);
   void SetDefaultToolTipType(const C_NagToolTip::E_Type oe_Type);
   void SetTemporaryToolTipHeading(const QString & orc_TemporaryToolTip);
   void SetTemporaryToolTipContent(const QString & orc_TemporaryToolTip);
   QString GetCurrentToolTipHeading(void) const;
   QString GetCurrentToolTipContent(void) const;
   C_NagToolTip::E_Type GetCurrentToolTipType(void) const;
   bool NotEmpty(void) const;
   virtual void GenerateHint(void) = 0;
   void DeactivateToolTipTemporarily(void);

protected:
   QString mc_DefaultToolTipHeading; ///< Default tool tip heading (Displayed in most situations, e.g. nothing
   ///< special is happening)
   QString mc_DefaultToolTipContent; ///< Default tool tip content (Displayed in most situations, e.g. nothing
   ///< special is happening)
   C_NagToolTip::E_Type me_DefaultToolTipType; ///< Current tool tip type (Displayed currently)
   QString mc_CurrentToolTipHeading;           ///< Current tool tip heading (Displayed currently)
   QString mc_CurrentToolTipContent;           ///< Current tool tip content (Displayed currently)
   C_NagToolTip::E_Type me_CurrentToolTipType; ///< Current tool tip type (Displayed currently)
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
