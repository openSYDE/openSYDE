//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node data pool table error manager (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTTABLEERRORMANAGER_H
#define C_SDNDEDPLISTTABLEERRORMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QObject>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListTableErrorManager :
   public QObject
{
   Q_OBJECT

public:
   C_SdNdeDpListTableErrorManager(void);
   void Init(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex, const uint32_t & oru32_ListIndex);
   void OnErrorChange(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChange(void);

private:
   uint32_t mu32_NodeIndex;
   uint32_t mu32_DataPoolIndex;
   uint32_t mu32_ListIndex;
   bool mq_ElementsInvalid;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
