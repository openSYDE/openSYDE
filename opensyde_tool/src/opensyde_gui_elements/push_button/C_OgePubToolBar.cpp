//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Tab Button in material design

   This class does not contain any functionality,
   but needs to exist, to have a unique group,
   to apply a specific stylesheet for.

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     21.07.2016  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QFile>
#include "C_OgePubToolBar.h"

/* -- Used Namespaces ------------------------------------------------------ */

using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ou32_Index  Unique identifier index for the button
   \param[in,out] opc_Parent  Optional pointer to parent

   \created     28.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgePubToolBar::C_OgePubToolBar(QWidget * const opc_Parent, const stw_types::uint32 ou32_Index) :
   C_OgePubStandard(opc_Parent),
   mu32_Index(ou32_Index)
{
   connect(this, &C_OgePubToolBar::clicked, this, &C_OgePubToolBar::m_ButtonClicked);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Send the index with click signal

   \created     06.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_OgePubToolBar::m_ButtonClicked(void)
{
   // send the index with click signal
   Q_EMIT this->SigClickedWithIndex(this->mu32_Index);
}
