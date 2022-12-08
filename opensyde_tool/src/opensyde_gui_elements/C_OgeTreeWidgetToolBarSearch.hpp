//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree widget for toolbar search (header)

   Tree widget for toolbar search (note: main module description should be in .cpp file)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGETREEWIDGETTOOLBARSEARCH_H
#define C_OGETREEWIDGETTOOLBARSEARCH_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeTreeWidgetToolBarSearch :
   public QTreeWidget
{
   Q_OBJECT

public:
   C_OgeTreeWidgetToolBarSearch(QWidget * const opc_Parent = NULL);
   virtual ~C_OgeTreeWidgetToolBarSearch();

   virtual bool Search(const QString & orc_SearchString) = 0;
   virtual void ClearResult(void) = 0;
   virtual void SetSearchResultFocus(void) = 0;
   bool HasResults(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChangeMode(const int32_t os32_Mode, const int32_t os32_SubMode, const uint32_t ou32_Index,
                      const QString & orc_Name, const QString & orc_SubSubItemName, const uint32_t ou32_Flag = 0U);
   void SigOpenDetail(const int32_t os32_Index, const int32_t os32_SubIndex, const int32_t os32_SubSubIndex,
                      const int32_t os32_SubSubSubIndex, const int32_t os32_Flag);

protected:
   void m_MarkResultString(QString & orc_ResultString) const;

   QString mc_SearchString;
   bool mq_ResultsFound;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
