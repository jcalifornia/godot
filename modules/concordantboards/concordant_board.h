/* concordant_board.h */
#ifndef CONCORDANT_BOARD_H
#define CONCORDANT_BOARD_H

#include "reference.h"
#include "resource.h"
#include <string>
#include <iostream>

class ConcordantBoard : public Reference {
    GDCLASS(ConcordantBoard, Reference);


protected:
    static void _bind_methods();

public:
    ConcordantBoard();
    void change_instrument();
    void draw(Point2 coord);
    int load_board();
    void to_svg(String filename);

};

#endif
