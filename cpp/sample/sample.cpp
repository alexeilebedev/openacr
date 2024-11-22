//
// License: GPL
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Target: sample (exe)
// Exceptions: yes
// Source: cpp/sample/sample.cpp
//

#include "include/algo.h"
#include "include/algo.h"
#include "include/sample.h"

void sample::CreateRecord(int i) {
    sample::FRec &rec = rec_Alloc();
    rec.rec.key1=i;
    rec.rec.key2=i+1;
    rec_XrefMaybe(rec);
    prlog("I created "<<rec.rec);
}

void sample::bh_rec_Step() {
    int r = random() % 100;
    if (r > 50) {
        prlog("delay by 10: "<<bh_rec_First()->rec);
        bh_rec_First()->rec.key2 += 10;
        bh_rec_Reheap(*bh_rec_First());
    } else if (r > 25 && bh_rec_N()>1) {
        prlog("process and delete: "<<bh_rec_First()->rec);
        rec_Delete(*bh_rec_First());
    } else {
        CreateRecord(random() % 10);
    }
}

void sample::Main() {
    for (int i=0; i<5; i++) {
        CreateRecord(i);
    }
    sample::Reckey key(0,1,"");
    if (sample::FRec *rec = ind_rec_Find(key)) {
        prlog("I found "<<rec->rec);
    }
    sample::MainLoop();
}
