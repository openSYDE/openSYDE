//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing search results out of the toolbar (header)

   Tool bar search result window

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_NAGTOOLBARSEARCHRESULTS_H
#define C_NAGTOOLBARSEARCHRESULTS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_NagToolBarSearchResults;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagToolBarSearchResults :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_NagToolBarSearchResults(QWidget * const opc_Parent = NULL);
   ~C_NagToolBarSearchResults();

   void StartSearch(const QString & orc_SearchString) const;
   void ClearResult(void) const;
   bool HasResults(void) const;
   void SetSearchResultFocus(void) const;
   bool HasChildFocus(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChangeMode(const stw_types::sint32 os32_Mode, const stw_types::sint32 os32_SubMode,
                      const stw_types::uint32 ou32_Index, const QString & orc_Name, const QString & orc_SubSubItemName,
                      const stw_types::uint32 ou32_Flag = 0U);
   void SigOpenDetail(const stw_types::sint32 os32_Index, const stw_types::sint32 os32_SubIndex,
                      const stw_types::sint32 os32_SubSubIndex, const stw_types::sint32 os32_SubSubSubIndex,
                      const stw_types::sint32 os32_Flag);
   void SigHide(void);
   void SigFocusOut(void);

private:
   //Avoid call
   C_NagToolBarSearchResults(const C_NagToolBarSearchResults &);
   C_NagToolBarSearchResults & operator =(const C_NagToolBarSearchResults &);

   Ui::C_NagToolBarSearchResults * mpc_Ui;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif // C_NAGTOOLTIP_H
