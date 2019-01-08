//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget with minimal progress bar. (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.10.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEWIPROGRESSBAR_H
#define C_OGEWIPROGRESSBAR_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include <QPen>

#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeWiProgressBar :
   public QWidget
{
   Q_OBJECT

public:
   C_OgeWiProgressBar(QWidget * const opc_Parent = NULL);

   void SetColorFree(const QColor & orc_Color);
   void SetColorReserved(const QColor & orc_Color);
   void SetColorTooMuch(const QColor & orc_Color);

   void SetProgress(const stw_types::uint32 ou32_Percentage, const bool oq_Full = false);
   stw_types::uint32 GetProgress(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   //lint -restore

private:
   QBrush mc_Brush;
   QPen mc_Pen;
   QColor mc_ColorFree;
   QColor mc_ColorReserved;
   QColor mc_ColorTooMuch;

   stw_types::uint32 mu32_Percentage;
   bool mq_Full;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
