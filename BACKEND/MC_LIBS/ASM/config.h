#define AmntCommands 50
#define LabelsAmnt 30

#define InvalidLabel -1

#define DoubleLineSize 20
#define IntegerLineSize 10

#define StdLabelLineSize 6 //label_
#define SimpleLabelLineSize 5 //label

#define StdSign 218
#define StdVersion 1

#define FIRST_ROUND 1

#define StdError 1

#define ExError -1
#define ExHlt 1


#define TEMPORARY_CONST_1 1
#define TEMPORARY_CONST_4 4


#define FirstFiveBitMask 31

enum EBits
{
    first_bit   = 1,
    second_bit  = 2,
    third_bit   = 4,
    fourth_bit  = 8,
    fifth_bit   = 16,
    sixth_bit   = 32,
    seventh_bit = 64,
    eighth_bit  = 128
};

enum EModes
{
    NRM_mode = 0,
    DBG_mode = 1
};

#define marker_mv 5

#define StartStackSize 20
#define StartAddressStackSize 20

#define Junk -1


