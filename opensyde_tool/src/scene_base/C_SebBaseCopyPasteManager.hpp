//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Copy paste manager of base scene (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBBASECOPYPASTEMANAGER_HPP
#define C_SEBBASECOPYPASTEMANAGER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QPointF>
#include <QList>
#include <QGraphicsItem>
#include "stwtypes.hpp"
#include "C_PuiBsElements.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
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
                                                  float64_t> & orc_NormalizedZetValues)
      =
         0;
   virtual bool CheckValidContentAndPrepareData(void) = 0;

   uint32_t GetPasteCounter(void) const;
   void IncrementPasteCounter(void);

protected:
   virtual void m_CalcOriginalPosition(const C_PuiBsElements * const opc_Data);
   static QPointF mh_Min(const QPointF & orc_P1, const QPointF & orc_P2);
   void m_MinToOrgPos(const QPointF & orc_Point);
   static void mh_HandleZetValueBox(const QGraphicsItem * const opc_Item, const QMap<const QGraphicsItem *,
                                                                                     float64_t> & orc_NormalizedZetValues, C_PuiBsBox & orc_Box);
   static void mh_HandleZetValueLine(const QGraphicsItem * const opc_Item, const QMap<const QGraphicsItem *,
                                                                                      float64_t> & orc_NormalizedZetValues, C_PuiBsLineBase & orc_Line);

   QPointF mc_OriginalPosition;
   uint32_t mu32_PasteCounter;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
