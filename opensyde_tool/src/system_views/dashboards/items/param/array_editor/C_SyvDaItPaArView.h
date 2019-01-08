//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list array edit table view (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     06.11.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVDAITPAARVIEW_H
#define C_SYVDAITPAARVIEW_H

/* -- Includes ------------------------------------------------------------- */
#include <QLabel>
#include "stwtypes.h"
#include "C_TblViewScroll.h"
#include "C_SyvDaItPaArModel.h"
#include "C_SyvDaItPaArDelegate.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvDaItPaArView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SyvDaItPaArView(QWidget * const opc_Parent = NULL);
   ~C_SyvDaItPaArView(void);
   void SetElement(const stw_types::uint32 ou32_ElementIndex,
                   stw_opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_DataWidget, const bool oq_ECUValues);
   void OnErrorChangePossible(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChangePossible(void);
   void SigSelectionChanged(const stw_types::uint32 & oru32_Count);

private:
   stw_opensyde_gui_logic::C_SyvDaItPaArModel mc_Model;
   stw_opensyde_gui_logic::C_SyvDaItPaArDelegate mc_Delegate;
   QLabel * mpc_LabelCorner;

   std::vector<stw_types::uint32> m_GetSelectedIndices(void) const;
   void m_UpdateCornerButton(void);
   //Avoid call
   C_SyvDaItPaArView(const C_SyvDaItPaArView &);
   C_SyvDaItPaArView & operator =(const C_SyvDaItPaArView &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
