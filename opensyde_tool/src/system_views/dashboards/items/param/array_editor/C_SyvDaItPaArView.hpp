//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list array edit table view (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITPAARVIEW_H
#define C_SYVDAITPAARVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QLabel>
#include "stwtypes.hpp"
#include "C_TblViewScroll.hpp"
#include "C_SyvDaItPaArModel.hpp"
#include "C_SyvDaItPaArDelegate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaItPaArView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SyvDaItPaArView(QWidget * const opc_Parent = NULL);
   ~C_SyvDaItPaArView(void) override;
   void SetElement(const uint32_t ou32_ElementIndex,
                   stw::opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_DataWidget,
                   const bool oq_EcuValues);
   void OnErrorChangePossible(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChangePossible(void);
   void SigSelectionChanged(const uint32_t & oru32_Count);

private:
   stw::opensyde_gui_logic::C_SyvDaItPaArModel mc_Model;
   stw::opensyde_gui_logic::C_SyvDaItPaArDelegate mc_Delegate;
   QLabel * mpc_LabelCorner;

   std::vector<uint32_t> m_GetSelectedIndices(void) const;
   void m_UpdateCornerButton(void);
   //Avoid call
   C_SyvDaItPaArView(const C_SyvDaItPaArView &);
   C_SyvDaItPaArView & operator =(const C_SyvDaItPaArView &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
