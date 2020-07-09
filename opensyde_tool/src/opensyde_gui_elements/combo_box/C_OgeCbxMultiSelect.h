//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Combo box with multi-selection functionality
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGECBXMULTISELECT_H
#define C_OGECBXMULTISELECT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.h"
#include "C_OgeCbxTableBase.h"

#include <QComboBox>
#include <QListWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeCbxMultiSelect :
   public C_OgeCbxTableBase
{
   Q_OBJECT

public:
   C_OgeCbxMultiSelect(QWidget * const opc_Parent = NULL);
   virtual ~C_OgeCbxMultiSelect();

   void SetDisplayText(const QString oc_Text);
   QString GetDisplayText() const;
   void RemoveAllItems();
   // add a item to the list
   void AddItem(const QString & orc_Text, const QVariant & orc_UserData = QVariant());
   // replace standard QComboBox Popup
   virtual void showPopup();
   virtual void hidePopup();

protected:
   // custom paint
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_OgeCbxMultiSelect(const C_OgeCbxMultiSelect &);
   C_OgeCbxMultiSelect & operator =(const C_OgeCbxMultiSelect &);

   // lower/upper screen bound
   stw_types::uint32 mu32_Screenbound;
   // hold the main display text
   QString mc_DisplayText;
   // popup frame
   QFrame * mpc_PopFrame;
   // multi selection list in the popup frame
   QListWidget * mpc_ListWidget;
   QListWidgetItem * mpc_ListWidgetItem;

   // react on changes of the item checkbox
   void m_ScanItemSelect(const QListWidgetItem * const opc_Item);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
