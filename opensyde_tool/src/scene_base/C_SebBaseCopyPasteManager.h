//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Copy paste manager of base scene (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBBASECOPYPASTEMANAGER_H
#define C_SEBBASECOPYPASTEMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPointF>
#include <QList>
#include <QGraphicsItem>
#include "stwtypes.h"
#include "C_PuiBsElements.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebBaseCopyPasteManager
{
public:
   C_SebBaseCopyPasteManager(void);
   virtual ~C_SebBaseCopyPasteManager(void);
   virtual const C_PuiBsElements * GetSnapshot(QWidget * const opc_Parent) = 0;
   QPointF GetDiff(const QPointF & orc_Position) const;
   virtual void CopyFromSceneToManager(const QList<QGraphicsItem *> & orc_SelectedItems,
                                       const QMap<const QGraphicsItem *,
                                                  stw_types::float64> & orc_NormalizedZValues)
      =
         0;
   virtual bool CheckValidContentAndPrepareData(void) = 0;

   stw_types::uint32 GetPasteCounter(void) const;
   void IncrementPasteCounter(void);

protected:
   virtual void m_CalcOriginalPosition(const C_PuiBsElements * const opc_Data);
   static QPointF mh_Min(const QPointF & orc_P1, const QPointF & orc_P2);
   void m_MinToOrgPos(const QPointF & orc_P);
   static void mh_HandleZValueBox(const QGraphicsItem * const opc_Item, const QMap<const QGraphicsItem *,
                                                                                   stw_types::float64> & orc_NormalizedZValues, C_PuiBsBox & orc_Box);
   static void mh_HandleZValueLine(const QGraphicsItem * const opc_Item, const QMap<const QGraphicsItem *,
                                                                                    stw_types::float64> & orc_NormalizedZValues, C_PuiBsLineBase & orc_Line);

   QPointF mc_OriginalPosition;
   stw_types::uint32 mu32_PasteCounter;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
