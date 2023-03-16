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
#include <QComboBox>
#include <QBitArray>
#include <QFrame>
#include <QListWidget>

#include "stwtypes.hpp"
#include "C_OgeCbxToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeCbxMultiSelect :
   public C_OgeCbxToolTipBase
{
   Q_OBJECT

public:
   C_OgeCbxMultiSelect(QWidget * const opc_Parent = NULL);
   ~C_OgeCbxMultiSelect() override;

   void SetDisplayText(const QString oc_Text);
   QString GetDisplayText() const;
   void RemoveAllItems();
   QBitArray GetValuesAsBitArray(void) const;
   // add a item to the list
   void AddItem(const QString & orc_Text, const QVariant & orc_UserData = QVariant());
   void SetItem(const QString & orc_DisplayName);
   void Init(const QStringList & orc_Strings, const QBitArray & orc_Values);
   // replace standard QComboBox Popup
   void showPopup() override;
   void hidePopup() override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigValueChanged(const QString & orc_ItemText, const bool oq_Checked);

protected:
   // custom paint
   void paintEvent(QPaintEvent * const opc_Event) override;
   void mousePressEvent(QMouseEvent * const opc_Event) override;

private:
   //Avoid call
   C_OgeCbxMultiSelect(const C_OgeCbxMultiSelect &);
   C_OgeCbxMultiSelect & operator =(const C_OgeCbxMultiSelect &) &;

   // hold the main display text
   QString mc_DisplayText;
   // popup frame
   QFrame * mpc_PopFrame;
   // multi selection list in the popup frame
   QListWidget * mpc_ListWidget;

   bool mq_IsShown;

   // react on changes of the item checkbox
   void m_ScanItemSelect(const QListWidgetItem * const opc_Item);
   void m_UpdateDisplayName(void);
   void m_ListWidgetItemClicked(const QModelIndex & orc_ModelIndex);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
