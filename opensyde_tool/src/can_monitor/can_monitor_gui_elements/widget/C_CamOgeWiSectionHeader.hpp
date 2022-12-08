//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Section header including a label and optional button for expand/collapse. (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGEWISECTIONHEADER_H
#define C_CAMOGEWISECTIONHEADER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiOnlyBackground.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamOgeWiSectionHeader;
}

namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgeWiSectionHeader :
   public C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   enum E_ButtonType
   {
      eNOBUTTON = 0,
      eUPDOWN,
      eLEFTRIGHT
   };

   explicit C_CamOgeWiSectionHeader(QWidget * const opc_Parent = NULL);
   virtual ~C_CamOgeWiSectionHeader(void);
   void SetIconType(const E_ButtonType & ore_ButtonType);
   void SetOpen(const bool & orq_IsOpen) const;
   void SetTitle(const QString & orc_Title) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigExpandSection(const bool & orq_Expand); // true: expand section, false: collapse section

private:
   Ui::C_CamOgeWiSectionHeader * mpc_Ui;
   E_ButtonType me_ButtonType;

   void m_OnExpandToggled(const bool oq_Expand);

   //Avoid call
   C_CamOgeWiSectionHeader(const C_CamOgeWiSectionHeader &);
   C_CamOgeWiSectionHeader & operator =(const C_CamOgeWiSectionHeader &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
