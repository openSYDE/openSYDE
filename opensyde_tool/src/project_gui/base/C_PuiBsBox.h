//-----------------------------------------------------------------------------
/*!
   \file
   \brief       UI base class for box based data: stores UI information (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     27.10.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_PUIBSBOXBASE_H
#define C_PUIBSBOXBASE_H

/* -- Includes ------------------------------------------------------------- */
#include <QPointF>
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_PuiBsBox
{
public:
   C_PuiBsBox();
   virtual ~C_PuiBsBox(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   QPointF c_UIPosition;          ///< Position of box based item in relation to scene
   stw_types::float64 f64_Width;  ///< Width of box based item in scene
   stw_types::float64 f64_Height; ///< Height of box based item in scene
   stw_types::float64 f64_ZOrder; ///< Z Position of box based item in scene
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
