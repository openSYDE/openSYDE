//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom combo box with text (header)

   Custom combo box with text (note: main module description should be in .cpp file)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECBXTEXT_HPP
#define C_OGECBXTEXT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

#include "C_OgeCbxIconOnly.hpp"
#include "C_OgeLeContextMenuBase.hpp"
/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeCbxTextLineEdit :
   public C_OgeLeContextMenuBase
{
   Q_OBJECT

public:
   C_OgeCbxTextLineEdit(QWidget * const opc_Parent = NULL);
};

class C_OgeCbxText :
   public C_OgeCbxIconOnly
{
   Q_OBJECT

public:
   C_OgeCbxText(QWidget * const opc_Parent = NULL);

   void SetTemporaryText(const QString & orc_Text, const bool oq_Error);

   void showPopup(void) override;
   void hidePopup(void) override;

   void SuppressHide();

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigErrorFixed(void);

private:
   QString mc_TemporaryText;
   int32_t ms32_TemporaryIndex;
   bool mq_Error;
   bool mq_SuppressHide;

   void m_Activated(const QString & orc_Text);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
