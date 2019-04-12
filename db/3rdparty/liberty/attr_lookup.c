/* ANSI-C code produced by gperf version 3.0.4 */
/* Command-line: gperf -L ANSI-C -t -H attr_hash_func -N lookup_attr_name -C -I
 * -k '*' -D attr_lookup  */

#if !(                                                                         \
    (' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) && ('%' == 37) && \
    ('&' == 38) && ('\'' == 39) && ('(' == 40) && (')' == 41) &&               \
    ('*' == 42) && ('+' == 43) && (',' == 44) && ('-' == 45) && ('.' == 46) && \
    ('/' == 47) && ('0' == 48) && ('1' == 49) && ('2' == 50) && ('3' == 51) && \
    ('4' == 52) && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) && \
    ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) && ('=' == 61) && \
    ('>' == 62) && ('?' == 63) && ('A' == 65) && ('B' == 66) && ('C' == 67) && \
    ('D' == 68) && ('E' == 69) && ('F' == 70) && ('G' == 71) && ('H' == 72) && \
    ('I' == 73) && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) && \
    ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) && ('R' == 82) && \
    ('S' == 83) && ('T' == 84) && ('U' == 85) && ('V' == 86) && ('W' == 87) && \
    ('X' == 88) && ('Y' == 89) && ('Z' == 90) && ('[' == 91) &&                \
    ('\\' == 92) && (']' == 93) && ('^' == 94) && ('_' == 95) &&               \
    ('a' == 97) && ('b' == 98) && ('c' == 99) && ('d' == 100) &&               \
    ('e' == 101) && ('f' == 102) && ('g' == 103) && ('h' == 104) &&            \
    ('i' == 105) && ('j' == 106) && ('k' == 107) && ('l' == 108) &&            \
    ('m' == 109) && ('n' == 110) && ('o' == 111) && ('p' == 112) &&            \
    ('q' == 113) && ('r' == 114) && ('s' == 115) && ('t' == 116) &&            \
    ('u' == 117) && ('v' == 118) && ('w' == 119) && ('x' == 120) &&            \
    ('y' == 121) && ('z' == 122) && ('{' == 123) && ('|' == 124) &&            \
    ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error \
    "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "attr_lookup"

#include "attr_enum.h"
#line 4 "attr_lookup"
struct libGroupMap {
        char *name;
        attr_enum type;
};
#include <string.h>

#define TOTAL_KEYWORDS 556
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 38
#define MIN_HASH_VALUE 4
#define MAX_HASH_VALUE 3759
/* maximum key range = 3756, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
    static unsigned int
    attr_hash_func(register const char *str, register unsigned int len) {
        static const unsigned short asso_values[] = {
            3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760,
            3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760,
            3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760,
            3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760,
            3760, 3760, 3760, 3760, 35,   20,   45,   25,   5,    0,    30,
            15,   3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760,
            3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760,
            3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760,
            3760, 3760, 3760, 3760, 3760, 3760, 3760, 10,   100,  0,    841,
            10,   50,   0,    5,    455,  445,  0,    285,  105,  0,    105,
            0,    35,   0,    240,  0,    0,    5,    350,  165,  980,  820,
            685,  100,  0,    3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760,
            3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760,
            3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760,
            3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760,
            3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760,
            3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760,
            3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760,
            3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760,
            3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760,
            3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760,
            3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760,
            3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760, 3760,
            3760, 3760, 3760, 3760};
        register int hval = len;

        switch (hval) {
                default:
                        hval += asso_values[(unsigned char)str[37]];
                /*FALLTHROUGH*/
                case 37:
                        hval += asso_values[(unsigned char)str[36]];
                /*FALLTHROUGH*/
                case 36:
                        hval += asso_values[(unsigned char)str[35]];
                /*FALLTHROUGH*/
                case 35:
                        hval += asso_values[(unsigned char)str[34]];
                /*FALLTHROUGH*/
                case 34:
                        hval += asso_values[(unsigned char)str[33]];
                /*FALLTHROUGH*/
                case 33:
                        hval += asso_values[(unsigned char)str[32]];
                /*FALLTHROUGH*/
                case 32:
                        hval += asso_values[(unsigned char)str[31]];
                /*FALLTHROUGH*/
                case 31:
                        hval += asso_values[(unsigned char)str[30]];
                /*FALLTHROUGH*/
                case 30:
                        hval += asso_values[(unsigned char)str[29]];
                /*FALLTHROUGH*/
                case 29:
                        hval += asso_values[(unsigned char)str[28]];
                /*FALLTHROUGH*/
                case 28:
                        hval += asso_values[(unsigned char)str[27]];
                /*FALLTHROUGH*/
                case 27:
                        hval += asso_values[(unsigned char)str[26]];
                /*FALLTHROUGH*/
                case 26:
                        hval += asso_values[(unsigned char)str[25]];
                /*FALLTHROUGH*/
                case 25:
                        hval += asso_values[(unsigned char)str[24]];
                /*FALLTHROUGH*/
                case 24:
                        hval += asso_values[(unsigned char)str[23]];
                /*FALLTHROUGH*/
                case 23:
                        hval += asso_values[(unsigned char)str[22]];
                /*FALLTHROUGH*/
                case 22:
                        hval += asso_values[(unsigned char)str[21]];
                /*FALLTHROUGH*/
                case 21:
                        hval += asso_values[(unsigned char)str[20]];
                /*FALLTHROUGH*/
                case 20:
                        hval += asso_values[(unsigned char)str[19]];
                /*FALLTHROUGH*/
                case 19:
                        hval += asso_values[(unsigned char)str[18]];
                /*FALLTHROUGH*/
                case 18:
                        hval += asso_values[(unsigned char)str[17]];
                /*FALLTHROUGH*/
                case 17:
                        hval += asso_values[(unsigned char)str[16]];
                /*FALLTHROUGH*/
                case 16:
                        hval += asso_values[(unsigned char)str[15]];
                /*FALLTHROUGH*/
                case 15:
                        hval += asso_values[(unsigned char)str[14]];
                /*FALLTHROUGH*/
                case 14:
                        hval += asso_values[(unsigned char)str[13]];
                /*FALLTHROUGH*/
                case 13:
                        hval += asso_values[(unsigned char)str[12]];
                /*FALLTHROUGH*/
                case 12:
                        hval += asso_values[(unsigned char)str[11]];
                /*FALLTHROUGH*/
                case 11:
                        hval += asso_values[(unsigned char)str[10]];
                /*FALLTHROUGH*/
                case 10:
                        hval += asso_values[(unsigned char)str[9]];
                /*FALLTHROUGH*/
                case 9:
                        hval += asso_values[(unsigned char)str[8]];
                /*FALLTHROUGH*/
                case 8:
                        hval += asso_values[(unsigned char)str[7] + 1];
                /*FALLTHROUGH*/
                case 7:
                        hval += asso_values[(unsigned char)str[6]];
                /*FALLTHROUGH*/
                case 6:
                        hval += asso_values[(unsigned char)str[5]];
                /*FALLTHROUGH*/
                case 5:
                        hval += asso_values[(unsigned char)str[4]];
                /*FALLTHROUGH*/
                case 4:
                        hval += asso_values[(unsigned char)str[3]];
                /*FALLTHROUGH*/
                case 3:
                        hval += asso_values[(unsigned char)str[2]];
                /*FALLTHROUGH*/
                case 2:
                        hval += asso_values[(unsigned char)str[1]];
                /*FALLTHROUGH*/
                case 1:
                        hval += asso_values[(unsigned char)str[0]];
                        break;
        }
        return hval;
}

#ifdef __GNUC__
__inline
#if defined __GNUC_STDC_INLINE__ || defined __GNUC_GNU_INLINE__
    __attribute__((__gnu_inline__))
#endif
#endif
    const struct libGroupMap *
    lookup_attr_name(register const char *str, register unsigned int len) {
        static const struct libGroupMap wordlist[] = {
#line 405 "attr_lookup"
            {"area", LIBERTY_ATTRENUM_area},
#line 245 "attr_lookup"
            {"preset", LIBERTY_ATTRENUM_preset},
#line 250 "attr_lookup"
            {"clear", LIBERTY_ATTRENUM_clear},
#line 200 "attr_lookup"
            {"cell_name", LIBERTY_ATTRENUM_cell_name},
#line 60 "attr_lookup"
            {"retention_pin", LIBERTY_ATTRENUM_retention_pin},
#line 184 "attr_lookup"
            {"slope", LIBERTY_ATTRENUM_slope},
#line 117 "attr_lookup"
            {"retention_cell", LIBERTY_ATTRENUM_retention_cell},
#line 270 "attr_lookup"
            {"rise_resistance", LIBERTY_ATTRENUM_rise_resistance},
#line 276 "attr_lookup"
            {"fall_resistance", LIBERTY_ATTRENUM_fall_resistance},
#line 198 "attr_lookup"
            {"process", LIBERTY_ATTRENUM_process},
#line 297 "attr_lookup"
            {"coefs", LIBERTY_ATTRENUM_coefs},
#line 558 "attr_lookup"
            {"date", LIBERTY_ATTRENUM_date},
#line 185 "attr_lookup"
            {"resistance", LIBERTY_ATTRENUM_resistance},
#line 368 "attr_lookup"
            {"pin_opposite", LIBERTY_ATTRENUM_pin_opposite},
#line 338 "attr_lookup"
            {"is_pad", LIBERTY_ATTRENUM_is_pad},
#line 68 "attr_lookup"
            {"lsi_pad", LIBERTY_ATTRENUM_lsi_pad},
#line 378 "attr_lookup"
            {"preferred", LIBERTY_ATTRENUM_preferred},
#line 236 "attr_lookup"
            {"data_in", LIBERTY_ATTRENUM_data_in},
#line 356 "attr_lookup"
            {"direction", LIBERTY_ATTRENUM_direction},
#line 403 "attr_lookup"
            {"cell_footprint", LIBERTY_ATTRENUM_cell_footprint},
#line 32 "attr_lookup"
            {"rise_pin_resistance", LIBERTY_ATTRENUM_rise_pin_resistance},
#line 298 "attr_lookup"
            {"orders", LIBERTY_ATTRENUM_orders},
#line 126 "attr_lookup"
            {"piece_define", LIBERTY_ATTRENUM_piece_define},
#line 34 "attr_lookup"
            {"fall_pin_resistance", LIBERTY_ATTRENUM_fall_pin_resistance},
#line 73 "attr_lookup"
            {"area_coefficient", LIBERTY_ATTRENUM_area_coefficient},
#line 285 "attr_lookup"
            {"address", LIBERTY_ATTRENUM_address},
#line 223 "attr_lookup"
            {"force_10", LIBERTY_ATTRENUM_force_10},
#line 375 "attr_lookup"
            {"set_node", LIBERTY_ATTRENUM_set_node},
#line 280 "attr_lookup"
            {"sdf_cond", LIBERTY_ATTRENUM_sdf_cond},
#line 81 "attr_lookup"
            {"reference_capacitance", LIBERTY_ATTRENUM_reference_capacitance},
#line 225 "attr_lookup"
            {"force_00", LIBERTY_ATTRENUM_force_00},
#line 56 "attr_lookup"
            {"is_needed", LIBERTY_ATTRENUM_is_needed},
#line 7 "attr_lookup"
            {"parameter5", LIBERTY_ATTRENUM_parameter5},
#line 226 "attr_lookup"
            {"tracks", LIBERTY_ATTRENUM_tracks},
#line 15 "attr_lookup"
            {"pin_names", LIBERTY_ATTRENUM_pin_names},
#line 8 "attr_lookup"
            {"parameter4", LIBERTY_ATTRENUM_parameter4},
#line 222 "attr_lookup"
            {"force_11", LIBERTY_ATTRENUM_force_11},
#line 129 "attr_lookup"
            {"piece_type", LIBERTY_ATTRENUM_piece_type},
#line 409 "attr_lookup"
            {"define_cell_area", LIBERTY_ATTRENUM_define_cell_area},
#line 268 "attr_lookup"
            {"sdf_cond_start", LIBERTY_ATTRENUM_sdf_cond_start},
#line 11 "attr_lookup"
            {"parameter1", LIBERTY_ATTRENUM_parameter1},
#line 224 "attr_lookup"
            {"force_01", LIBERTY_ATTRENUM_force_01},
#line 9 "attr_lookup"
            {"parameter3", LIBERTY_ATTRENUM_parameter3},
#line 367 "attr_lookup"
            {"rail_connection", LIBERTY_ATTRENUM_rail_connection},
#line 90 "attr_lookup"
            {"isolation_cell_data_pin",
             LIBERTY_ATTRENUM_isolation_cell_data_pin},
#line 365 "attr_lookup"
            {"clock", LIBERTY_ATTRENUM_clock},
#line 304 "attr_lookup"
            {"related_pin", LIBERTY_ATTRENUM_related_pin},
#line 203 "attr_lookup"
            {"vil", LIBERTY_ATTRENUM_vil},
#line 10 "attr_lookup"
            {"parameter2", LIBERTY_ATTRENUM_parameter2},
#line 133 "attr_lookup"
            {"lsi_pad_rise", LIBERTY_ATTRENUM_lsi_pad_rise},
#line 241 "attr_lookup"
            {"invert", LIBERTY_ATTRENUM_invert},
#line 134 "attr_lookup"
            {"lsi_pad_fall", LIBERTY_ATTRENUM_lsi_pad_fall},
#line 74 "attr_lookup"
            {"calc_mode", LIBERTY_ATTRENUM_calc_mode},
#line 269 "attr_lookup"
            {"sdf_cond_end", LIBERTY_ATTRENUM_sdf_cond_end},
#line 380 "attr_lookup"
            {"pad_cell", LIBERTY_ATTRENUM_pad_cell},
#line 112 "attr_lookup"
            {"sensitization_master", LIBERTY_ATTRENUM_sensitization_master},
#line 31 "attr_lookup"
            {"reference_time", LIBERTY_ATTRENUM_reference_time},
#line 53 "attr_lookup"
            {"miller_cap_rise", LIBERTY_ATTRENUM_miller_cap_rise},
#line 125 "attr_lookup"
            {"va_parameters", LIBERTY_ATTRENUM_va_parameters},
#line 254 "attr_lookup"
            {"mode", LIBERTY_ATTRENUM_mode},
#line 52 "attr_lookup"
            {"miller_cap_fall", LIBERTY_ATTRENUM_miller_cap_fall},
#line 227 "attr_lookup"
            {"total_track_area", LIBERTY_ATTRENUM_total_track_area},
#line 192 "attr_lookup"
            {"vol", LIBERTY_ATTRENUM_vol},
#line 328 "attr_lookup"
            {"min_transition", LIBERTY_ATTRENUM_min_transition},
#line 504 "attr_lookup"
            {"k_process_pin_cap", LIBERTY_ATTRENUM_k_process_pin_cap},
#line 165 "attr_lookup"
            {"k_process_cell_rise", LIBERTY_ATTRENUM_k_process_cell_rise},
#line 340 "attr_lookup"
            {"internal_node", LIBERTY_ATTRENUM_internal_node},
#line 166 "attr_lookup"
            {"k_process_cell_fall", LIBERTY_ATTRENUM_k_process_cell_fall},
#line 14 "attr_lookup"
            {"vector", LIBERTY_ATTRENUM_vector},
#line 510 "attr_lookup"
            {"k_process_intrinsic_rise",
             LIBERTY_ATTRENUM_k_process_intrinsic_rise},
#line 502 "attr_lookup"
            {"k_process_pin_rise_cap", LIBERTY_ATTRENUM_k_process_pin_rise_cap},
#line 511 "attr_lookup"
            {"k_process_intrinsic_fall",
             LIBERTY_ATTRENUM_k_process_intrinsic_fall},
#line 503 "attr_lookup"
            {"k_process_pin_fall_cap", LIBERTY_ATTRENUM_k_process_pin_fall_cap},
#line 249 "attr_lookup"
            {"clear_preset_var1", LIBERTY_ATTRENUM_clear_preset_var1},
#line 159 "attr_lookup"
            {"k_process_rise_pin_resistance",
             LIBERTY_ATTRENUM_k_process_rise_pin_resistance},
#line 492 "attr_lookup"
            {"k_process_slope_rise", LIBERTY_ATTRENUM_k_process_slope_rise},
#line 420 "attr_lookup"
            {"revision", LIBERTY_ATTRENUM_revision},
#line 163 "attr_lookup"
            {"k_process_fall_pin_resistance",
             LIBERTY_ATTRENUM_k_process_fall_pin_resistance},
#line 493 "attr_lookup"
            {"k_process_slope_fall", LIBERTY_ATTRENUM_k_process_slope_fall},
#line 429 "attr_lookup"
            {"nom_process", LIBERTY_ATTRENUM_nom_process},
#line 108 "attr_lookup"
            {"pin_name_map", LIBERTY_ATTRENUM_pin_name_map},
#line 311 "attr_lookup"
            {"state_function", LIBERTY_ATTRENUM_state_function},
#line 157 "attr_lookup"
            {"k_process_rise_transition",
             LIBERTY_ATTRENUM_k_process_rise_transition},
#line 384 "attr_lookup"
            {"map_only", LIBERTY_ATTRENUM_map_only},
#line 161 "attr_lookup"
            {"k_process_fall_transition",
             LIBERTY_ATTRENUM_k_process_fall_transition},
#line 248 "attr_lookup"
            {"clear_preset_var2", LIBERTY_ATTRENUM_clear_preset_var2},
#line 560 "attr_lookup"
            {"comment", LIBERTY_ATTRENUM_comment},
#line 201 "attr_lookup"
            {"vimin", LIBERTY_ATTRENUM_vimin},
#line 88 "attr_lookup"
            {"level_shifter_data_pin", LIBERTY_ATTRENUM_level_shifter_data_pin},
#line 399 "attr_lookup"
            {"dont_fault", LIBERTY_ATTRENUM_dont_fault},
#line 148 "attr_lookup"
            {"k_temp_rise_pin_resistance",
             LIBERTY_ATTRENUM_k_temp_rise_pin_resistance},
#line 318 "attr_lookup"
            {"rise_capacitance", LIBERTY_ATTRENUM_rise_capacitance},
#line 465 "attr_lookup"
            {"k_temp_slope_rise", LIBERTY_ATTRENUM_k_temp_slope_rise},
#line 353 "attr_lookup"
            {"fall_capacitance", LIBERTY_ATTRENUM_fall_capacitance},
#line 466 "attr_lookup"
            {"k_temp_slope_fall", LIBERTY_ATTRENUM_k_temp_slope_fall},
#line 190 "attr_lookup"
            {"vomin", LIBERTY_ATTRENUM_vomin},
#line 154 "attr_lookup"
            {"k_temp_cell_rise", LIBERTY_ATTRENUM_k_temp_cell_rise},
#line 146 "attr_lookup"
            {"k_temp_rise_transition", LIBERTY_ATTRENUM_k_temp_rise_transition},
#line 155 "attr_lookup"
            {"k_temp_cell_fall", LIBERTY_ATTRENUM_k_temp_cell_fall},
#line 104 "attr_lookup"
            {"std_cell_main_rail", LIBERTY_ATTRENUM_std_cell_main_rail},
#line 273 "attr_lookup"
            {"intrinsic_rise", LIBERTY_ATTRENUM_intrinsic_rise},
#line 274 "attr_lookup"
            {"intrinsic_fall", LIBERTY_ATTRENUM_intrinsic_fall},
#line 382 "attr_lookup"
            {"mpm_name", LIBERTY_ATTRENUM_mpm_name},
#line 265 "attr_lookup"
            {"slope_rise", LIBERTY_ATTRENUM_slope_rise},
#line 484 "attr_lookup"
            {"k_temp_hold_rise", LIBERTY_ATTRENUM_k_temp_hold_rise},
#line 480 "attr_lookup"
            {"k_temp_min_period", LIBERTY_ATTRENUM_k_temp_min_period},
#line 279 "attr_lookup"
            {"constraint", LIBERTY_ATTRENUM_constraint},
#line 485 "attr_lookup"
            {"k_temp_hold_fall", LIBERTY_ATTRENUM_k_temp_hold_fall},
#line 284 "attr_lookup"
            {"clocked_on", LIBERTY_ATTRENUM_clocked_on},
#line 240 "attr_lookup"
            {"master_pin", LIBERTY_ATTRENUM_master_pin},
#line 137 "attr_lookup"
            {"k_volt_rise_pin_resistance",
             LIBERTY_ATTRENUM_k_volt_rise_pin_resistance},
#line 509 "attr_lookup"
            {"k_process_min_period", LIBERTY_ATTRENUM_k_process_min_period},
#line 438 "attr_lookup"
            {"k_volt_slope_rise", LIBERTY_ATTRENUM_k_volt_slope_rise},
#line 439 "attr_lookup"
            {"k_volt_slope_fall", LIBERTY_ATTRENUM_k_volt_slope_fall},
#line 247 "attr_lookup"
            {"clocked_on_also", LIBERTY_ATTRENUM_clocked_on_also},
#line 143 "attr_lookup"
            {"k_volt_cell_rise", LIBERTY_ATTRENUM_k_volt_cell_rise},
#line 135 "attr_lookup"
            {"k_volt_rise_transition", LIBERTY_ATTRENUM_k_volt_rise_transition},
#line 144 "attr_lookup"
            {"k_volt_cell_fall", LIBERTY_ATTRENUM_k_volt_cell_fall},
#line 358 "attr_lookup"
            {"connection_class", LIBERTY_ATTRENUM_connection_class},
#line 43 "attr_lookup"
            {"setup_coefficient", LIBERTY_ATTRENUM_setup_coefficient},
#line 355 "attr_lookup"
            {"drive_current", LIBERTY_ATTRENUM_drive_current},
#line 515 "attr_lookup"
            {"k_process_drive_rise", LIBERTY_ATTRENUM_k_process_drive_rise},
#line 130 "attr_lookup"
            {"nom_calc_mode", LIBERTY_ATTRENUM_nom_calc_mode},
#line 516 "attr_lookup"
            {"k_process_drive_fall", LIBERTY_ATTRENUM_k_process_drive_fall},
#line 322 "attr_lookup"
            {"prefer_tied", LIBERTY_ATTRENUM_prefer_tied},
#line 486 "attr_lookup"
            {"k_temp_drive_rise", LIBERTY_ATTRENUM_k_temp_drive_rise},
#line 127 "attr_lookup"
            {"resistance_unit", LIBERTY_ATTRENUM_resistance_unit},
#line 487 "attr_lookup"
            {"k_temp_drive_fall", LIBERTY_ATTRENUM_k_temp_drive_fall},
#line 44 "attr_lookup"
            {"interdependence_id", LIBERTY_ATTRENUM_interdependence_id},
#line 179 "attr_lookup"
            {"fanout_resistance", LIBERTY_ATTRENUM_fanout_resistance},
#line 457 "attr_lookup"
            {"k_volt_hold_rise", LIBERTY_ATTRENUM_k_volt_hold_rise},
#line 453 "attr_lookup"
            {"k_volt_min_period", LIBERTY_ATTRENUM_k_volt_min_period},
#line 458 "attr_lookup"
            {"k_volt_hold_fall", LIBERTY_ATTRENUM_k_volt_hold_fall},
#line 345 "attr_lookup"
            {"function", LIBERTY_ATTRENUM_function},
#line 237 "attr_lookup"
            {"shifts", LIBERTY_ATTRENUM_shifts},
#line 397 "attr_lookup"
            {"dont_use", LIBERTY_ATTRENUM_dont_use},
#line 244 "attr_lookup"
            {"clock_pin", LIBERTY_ATTRENUM_clock_pin},
#line 123 "attr_lookup"
            {"failure_rate", LIBERTY_ATTRENUM_failure_rate},
#line 278 "attr_lookup"
            {"fsim_map", LIBERTY_ATTRENUM_fsim_map},
#line 332 "attr_lookup"
            {"min_fanout", LIBERTY_ATTRENUM_min_fanout},
#line 199 "attr_lookup"
            {"short", LIBERTY_ATTRENUM_short},
#line 331 "attr_lookup"
            {"min_period", LIBERTY_ATTRENUM_min_period},
#line 180 "attr_lookup"
            {"fanout_capacitance", LIBERTY_ATTRENUM_fanout_capacitance},
#line 91 "attr_lookup"
            {"is_three_state", LIBERTY_ATTRENUM_is_three_state},
#line 475 "attr_lookup"
            {"k_temp_pin_cap", LIBERTY_ATTRENUM_k_temp_pin_cap},
#line 238 "attr_lookup"
            {"edges", LIBERTY_ATTRENUM_edges},
#line 498 "attr_lookup"
            {"k_process_removal_rise", LIBERTY_ATTRENUM_k_process_removal_rise},
#line 499 "attr_lookup"
            {"k_process_removal_fall", LIBERTY_ATTRENUM_k_process_removal_fall},
#line 234 "attr_lookup"
            {"value", LIBERTY_ATTRENUM_value},
#line 299 "attr_lookup"
            {"values", LIBERTY_ATTRENUM_values},
#line 459 "attr_lookup"
            {"k_volt_drive_rise", LIBERTY_ATTRENUM_k_volt_drive_rise},
#line 293 "attr_lookup"
            {"related_input", LIBERTY_ATTRENUM_related_input},
#line 292 "attr_lookup"
            {"related_inputs", LIBERTY_ATTRENUM_related_inputs},
#line 460 "attr_lookup"
            {"k_volt_drive_fall", LIBERTY_ATTRENUM_k_volt_drive_fall},
#line 347 "attr_lookup"
            {"fault_model", LIBERTY_ATTRENUM_fault_model},
#line 187 "attr_lookup"
            {"faster_factor", LIBERTY_ATTRENUM_faster_factor},
#line 400 "attr_lookup"
            {"contention_condition", LIBERTY_ATTRENUM_contention_condition},
#line 18 "attr_lookup"
            {"curve_id", LIBERTY_ATTRENUM_curve_id},
#line 547 "attr_lookup"
            {"default_intrinsic_rise", LIBERTY_ATTRENUM_default_intrinsic_rise},
#line 496 "attr_lookup"
            {"k_process_setup_rise", LIBERTY_ATTRENUM_k_process_setup_rise},
#line 548 "attr_lookup"
            {"default_intrinsic_fall", LIBERTY_ATTRENUM_default_intrinsic_fall},
#line 497 "attr_lookup"
            {"k_process_setup_fall", LIBERTY_ATTRENUM_k_process_setup_fall},
#line 481 "attr_lookup"
            {"k_temp_intrinsic_rise", LIBERTY_ATTRENUM_k_temp_intrinsic_rise},
#line 207 "attr_lookup"
            {"mapping", LIBERTY_ATTRENUM_mapping},
#line 38 "attr_lookup"
            {"tied_off", LIBERTY_ATTRENUM_tied_off},
#line 471 "attr_lookup"
            {"k_temp_removal_rise", LIBERTY_ATTRENUM_k_temp_removal_rise},
#line 482 "attr_lookup"
            {"k_temp_intrinsic_fall", LIBERTY_ATTRENUM_k_temp_intrinsic_fall},
#line 535 "attr_lookup"
            {"default_rise_pin_resistance",
             LIBERTY_ATTRENUM_default_rise_pin_resistance},
#line 533 "attr_lookup"
            {"default_slope_rise", LIBERTY_ATTRENUM_default_slope_rise},
#line 472 "attr_lookup"
            {"k_temp_removal_fall", LIBERTY_ATTRENUM_k_temp_removal_fall},
#line 554 "attr_lookup"
            {"default_fall_pin_resistance",
             LIBERTY_ATTRENUM_default_fall_pin_resistance},
#line 534 "attr_lookup"
            {"default_slope_fall", LIBERTY_ATTRENUM_default_slope_fall},
#line 267 "attr_lookup"
            {"sdf_edges", LIBERTY_ATTRENUM_sdf_edges},
#line 55 "attr_lookup"
            {"stage_type", LIBERTY_ATTRENUM_stage_type},
#line 258 "attr_lookup"
            {"timing_sense", LIBERTY_ATTRENUM_timing_sense},
#line 170 "attr_lookup"
            {"default_reference_capacitance",
             LIBERTY_ATTRENUM_default_reference_capacitance},
#line 524 "attr_lookup"
            {"force_inverter_removal", LIBERTY_ATTRENUM_force_inverter_removal},
#line 448 "attr_lookup"
            {"k_volt_pin_cap", LIBERTY_ATTRENUM_k_volt_pin_cap},
#line 348 "attr_lookup"
            {"fanout_load", LIBERTY_ATTRENUM_fanout_load},
#line 204 "attr_lookup"
            {"vih", LIBERTY_ATTRENUM_vih},
#line 469 "attr_lookup"
            {"k_temp_setup_rise", LIBERTY_ATTRENUM_k_temp_setup_rise},
#line 470 "attr_lookup"
            {"k_temp_setup_fall", LIBERTY_ATTRENUM_k_temp_setup_fall},
#line 412 "attr_lookup"
            {"timing_report", LIBERTY_ATTRENUM_timing_report},
#line 45 "attr_lookup"
            {"hold_coefficient", LIBERTY_ATTRENUM_hold_coefficient},
#line 69 "attr_lookup"
            {"related_pg_pin", LIBERTY_ATTRENUM_related_pg_pin},
#line 377 "attr_lookup"
            {"scaling_factors", LIBERTY_ATTRENUM_scaling_factors},
#line 171 "attr_lookup"
            {"default_rc_rise_coefficient",
             LIBERTY_ATTRENUM_default_rc_rise_coefficient},
#line 290 "attr_lookup"
            {"related_rising_pin", LIBERTY_ATTRENUM_related_rising_pin},
#line 172 "attr_lookup"
            {"default_rc_fall_coefficient",
             LIBERTY_ATTRENUM_default_rc_fall_coefficient},
#line 294 "attr_lookup"
            {"related_falling_pin", LIBERTY_ATTRENUM_related_falling_pin},
#line 54 "attr_lookup"
            {"is_inverting", LIBERTY_ATTRENUM_is_inverting},
#line 193 "attr_lookup"
            {"voh", LIBERTY_ATTRENUM_voh},
#line 556 "attr_lookup"
            {"default_connection_class",
             LIBERTY_ATTRENUM_default_connection_class},
#line 386 "attr_lookup"
            {"interface_timing", LIBERTY_ATTRENUM_interface_timing},
#line 20 "attr_lookup"
            {"related_internal_pg_pin",
             LIBERTY_ATTRENUM_related_internal_pg_pin},
#line 300 "attr_lookup"
            {"intermediate_values", LIBERTY_ATTRENUM_intermediate_values},
#line 233 "attr_lookup"
            {"input_pins", LIBERTY_ATTRENUM_input_pins},
#line 454 "attr_lookup"
            {"k_volt_intrinsic_rise", LIBERTY_ATTRENUM_k_volt_intrinsic_rise},
#line 327 "attr_lookup"
            {"multicell_pad_pin", LIBERTY_ATTRENUM_multicell_pad_pin},
#line 444 "attr_lookup"
            {"k_volt_removal_rise", LIBERTY_ATTRENUM_k_volt_removal_rise},
#line 455 "attr_lookup"
            {"k_volt_intrinsic_fall", LIBERTY_ATTRENUM_k_volt_intrinsic_fall},
#line 195 "attr_lookup"
            {"voltage", LIBERTY_ATTRENUM_voltage},
#line 445 "attr_lookup"
            {"k_volt_removal_fall", LIBERTY_ATTRENUM_k_volt_removal_fall},
#line 372 "attr_lookup"
            {"vhdl_name", LIBERTY_ATTRENUM_vhdl_name},
#line 229 "attr_lookup"
            {"type", LIBERTY_ATTRENUM_type},
#line 29 "attr_lookup"
            {"va_values", LIBERTY_ATTRENUM_va_values},
#line 442 "attr_lookup"
            {"k_volt_setup_rise", LIBERTY_ATTRENUM_k_volt_setup_rise},
#line 443 "attr_lookup"
            {"k_volt_setup_fall", LIBERTY_ATTRENUM_k_volt_setup_fall},
#line 388 "attr_lookup"
            {"geometry_print", LIBERTY_ATTRENUM_geometry_print},
#line 513 "attr_lookup"
            {"k_process_hold_rise", LIBERTY_ATTRENUM_k_process_hold_rise},
#line 514 "attr_lookup"
            {"k_process_hold_fall", LIBERTY_ATTRENUM_k_process_hold_fall},
#line 158 "attr_lookup"
            {"k_process_rise_propagation",
             LIBERTY_ATTRENUM_k_process_rise_propagation},
#line 162 "attr_lookup"
            {"k_process_fall_propagation",
             LIBERTY_ATTRENUM_k_process_fall_propagation},
#line 152 "attr_lookup"
            {"k_temp_fall_pin_resistance",
             LIBERTY_ATTRENUM_k_temp_fall_pin_resistance},
#line 120 "attr_lookup"
            {"is_level_shifter", LIBERTY_ATTRENUM_is_level_shifter},
#line 379 "attr_lookup"
            {"pad_type", LIBERTY_ATTRENUM_pad_type},
#line 150 "attr_lookup"
            {"k_temp_fall_transition", LIBERTY_ATTRENUM_k_temp_fall_transition},
#line 413 "attr_lookup"
            {"time_unit", LIBERTY_ATTRENUM_time_unit},
#line 398 "attr_lookup"
            {"dont_touch", LIBERTY_ATTRENUM_dont_touch},
#line 59 "attr_lookup"
            {"rise_capacitance_range", LIBERTY_ATTRENUM_rise_capacitance_range},
#line 61 "attr_lookup"
            {"fall_capacitance_range", LIBERTY_ATTRENUM_fall_capacitance_range},
#line 86 "attr_lookup"
            {"map_to_logic", LIBERTY_ATTRENUM_map_to_logic},
#line 517 "attr_lookup"
            {"k_process_drive_current",
             LIBERTY_ATTRENUM_k_process_drive_current},
#line 419 "attr_lookup"
            {"simulation", LIBERTY_ATTRENUM_simulation},
#line 488 "attr_lookup"
            {"k_temp_drive_current", LIBERTY_ATTRENUM_k_temp_drive_current},
#line 147 "attr_lookup"
            {"k_temp_rise_propagation",
             LIBERTY_ATTRENUM_k_temp_rise_propagation},
#line 432 "attr_lookup"
            {"key_seed", LIBERTY_ATTRENUM_key_seed},
#line 167 "attr_lookup"
            {"k_process_cell_degradation",
             LIBERTY_ATTRENUM_k_process_cell_degradation},
#line 433 "attr_lookup"
            {"key_file", LIBERTY_ATTRENUM_key_file},
#line 197 "attr_lookup"
            {"temperature", LIBERTY_ATTRENUM_temperature},
#line 309 "attr_lookup"
            {"three_state", LIBERTY_ATTRENUM_three_state},
#line 559 "attr_lookup"
            {"current_unit", LIBERTY_ATTRENUM_current_unit},
#line 376 "attr_lookup"
            {"scan_group", LIBERTY_ATTRENUM_scan_group},
#line 113 "attr_lookup"
            {"user_function_class", LIBERTY_ATTRENUM_user_function_class},
#line 141 "attr_lookup"
            {"k_volt_fall_pin_resistance",
             LIBERTY_ATTRENUM_k_volt_fall_pin_resistance},
#line 283 "attr_lookup"
            {"enable", LIBERTY_ATTRENUM_enable},
#line 221 "attr_lookup"
            {"table", LIBERTY_ATTRENUM_table},
#line 28 "attr_lookup"
            {"typical_capacitances", LIBERTY_ATTRENUM_typical_capacitances},
#line 139 "attr_lookup"
            {"k_volt_fall_transition", LIBERTY_ATTRENUM_k_volt_fall_transition},
#line 428 "attr_lookup"
            {"nom_temperature", LIBERTY_ATTRENUM_nom_temperature},
#line 124 "attr_lookup"
            {"voltage_map", LIBERTY_ATTRENUM_voltage_map},
#line 105 "attr_lookup"
            {"voltage_name", LIBERTY_ATTRENUM_voltage_name},
#line 366 "attr_lookup"
            {"capacitance", LIBERTY_ATTRENUM_capacitance},
#line 93 "attr_lookup"
            {"edge_rate_rise", LIBERTY_ATTRENUM_edge_rate_rise},
#line 461 "attr_lookup"
            {"k_volt_drive_current", LIBERTY_ATTRENUM_k_volt_drive_current},
#line 51 "attr_lookup"
            {"index_4", LIBERTY_ATTRENUM_index_4},
#line 136 "attr_lookup"
            {"k_volt_rise_propagation",
             LIBERTY_ATTRENUM_k_volt_rise_propagation},
#line 96 "attr_lookup"
            {"edge_rate_fall", LIBERTY_ATTRENUM_edge_rate_fall},
#line 33 "attr_lookup"
            {"rise_delay_intercept", LIBERTY_ATTRENUM_rise_delay_intercept},
#line 266 "attr_lookup"
            {"slope_fall", LIBERTY_ATTRENUM_slope_fall},
#line 218 "attr_lookup"
            {"bit_to", LIBERTY_ATTRENUM_bit_to},
#line 119 "attr_lookup"
            {"level_shifter_type", LIBERTY_ATTRENUM_level_shifter_type},
#line 219 "attr_lookup"
            {"bit_from", LIBERTY_ATTRENUM_bit_from},
#line 35 "attr_lookup"
            {"fall_delay_intercept", LIBERTY_ATTRENUM_fall_delay_intercept},
#line 303 "attr_lookup"
            {"index_1", LIBERTY_ATTRENUM_index_1},
#line 252 "attr_lookup"
            {"tdisable", LIBERTY_ATTRENUM_tdisable},
#line 526 "attr_lookup"
            {"delay_model", LIBERTY_ATTRENUM_delay_model},
#line 301 "attr_lookup"
            {"index_3", LIBERTY_ATTRENUM_index_3},
#line 156 "attr_lookup"
            {"k_temp_cell_degradation",
             LIBERTY_ATTRENUM_k_temp_cell_degradation},
#line 24 "attr_lookup"
            {"related_output", LIBERTY_ATTRENUM_related_output},
#line 291 "attr_lookup"
            {"related_outputs", LIBERTY_ATTRENUM_related_outputs},
#line 549 "attr_lookup"
            {"default_input_pin_cap", LIBERTY_ATTRENUM_default_input_pin_cap},
#line 178 "attr_lookup"
            {"default_cell_failure_rate",
             LIBERTY_ATTRENUM_default_cell_failure_rate},
#line 271 "attr_lookup"
            {"related_output_pin", LIBERTY_ATTRENUM_related_output_pin},
#line 302 "attr_lookup"
            {"index_2", LIBERTY_ATTRENUM_index_2},
#line 319 "attr_lookup"
            {"pulling_resistance", LIBERTY_ATTRENUM_pulling_resistance},
#line 339 "attr_lookup"
            {"inverted_output", LIBERTY_ATTRENUM_inverted_output},
#line 89 "attr_lookup"
            {"isolation_cell_enable_pin",
             LIBERTY_ATTRENUM_isolation_cell_enable_pin},
#line 30 "attr_lookup"
            {"nominal_va_values", LIBERTY_ATTRENUM_nominal_va_values},
#line 552 "attr_lookup"
            {"default_inout_pin_cap", LIBERTY_ATTRENUM_default_inout_pin_cap},
#line 196 "attr_lookup"
            {"tree_type", LIBERTY_ATTRENUM_tree_type},
#line 410 "attr_lookup"
            {"capacitive_load_unit", LIBERTY_ATTRENUM_capacitive_load_unit},
#line 550 "attr_lookup"
            {"default_inout_pin_rise_res",
             LIBERTY_ATTRENUM_default_inout_pin_rise_res},
#line 519 "attr_lookup"
            {"input_threshold_pct_rise",
             LIBERTY_ATTRENUM_input_threshold_pct_rise},
#line 551 "attr_lookup"
            {"default_inout_pin_fall_res",
             LIBERTY_ATTRENUM_default_inout_pin_fall_res},
#line 121 "attr_lookup"
            {"is_isolation_cell", LIBERTY_ATTRENUM_is_isolation_cell},
#line 520 "attr_lookup"
            {"input_threshold_pct_fall",
             LIBERTY_ATTRENUM_input_threshold_pct_fall},
#line 169 "attr_lookup"
            {"default_setup_coefficient",
             LIBERTY_ATTRENUM_default_setup_coefficient},
#line 359 "attr_lookup"
            {"complementary_pin", LIBERTY_ATTRENUM_complementary_pin},
#line 160 "attr_lookup"
            {"k_process_rise_delay_intercept",
             LIBERTY_ATTRENUM_k_process_rise_delay_intercept},
#line 164 "attr_lookup"
            {"k_process_fall_delay_intercept",
             LIBERTY_ATTRENUM_k_process_fall_delay_intercept},
#line 94 "attr_lookup"
            {"edge_rate_load_rise", LIBERTY_ATTRENUM_edge_rate_load_rise},
#line 77 "attr_lookup"
            {"poly_convert", LIBERTY_ATTRENUM_poly_convert},
#line 263 "attr_lookup"
            {"steady_state_resistance_float_min",
             LIBERTY_ATTRENUM_steady_state_resistance_float_min},
#line 95 "attr_lookup"
            {"edge_rate_load_fall", LIBERTY_ATTRENUM_edge_rate_load_fall},
#line 341 "attr_lookup"
            {"input_voltage", LIBERTY_ATTRENUM_input_voltage},
#line 216 "attr_lookup"
            {"data_type", LIBERTY_ATTRENUM_data_type},
#line 333 "attr_lookup"
            {"min_capacitance", LIBERTY_ATTRENUM_min_capacitance},
#line 145 "attr_lookup"
            {"k_volt_cell_degradation",
             LIBERTY_ATTRENUM_k_volt_cell_degradation},
#line 525 "attr_lookup"
            {"em_temp_degradation_factor",
             LIBERTY_ATTRENUM_em_temp_degradation_factor},
#line 431 "attr_lookup"
            {"key_version", LIBERTY_ATTRENUM_key_version},
#line 58 "attr_lookup"
            {"low", LIBERTY_ATTRENUM_low},
#line 553 "attr_lookup"
            {"default_fanout_load", LIBERTY_ATTRENUM_default_fanout_load},
#line 214 "attr_lookup"
            {"variables", LIBERTY_ATTRENUM_variables},
#line 374 "attr_lookup"
            {"single_bit_degenerate", LIBERTY_ATTRENUM_single_bit_degenerate},
#line 334 "attr_lookup"
            {"max_transition", LIBERTY_ATTRENUM_max_transition},
#line 385 "attr_lookup"
            {"is_clock_gating_cell", LIBERTY_ATTRENUM_is_clock_gating_cell},
#line 357 "attr_lookup"
            {"dcm_timing", LIBERTY_ATTRENUM_dcm_timing},
#line 12 "attr_lookup"
            {"variable_4", LIBERTY_ATTRENUM_variable_4},
#line 149 "attr_lookup"
            {"k_temp_rise_delay_intercept",
             LIBERTY_ATTRENUM_k_temp_rise_delay_intercept},
#line 315 "attr_lookup"
            {"rise_time_after_threshold",
             LIBERTY_ATTRENUM_rise_time_after_threshold},
#line 206 "attr_lookup"
            {"variable_1", LIBERTY_ATTRENUM_variable_1},
#line 350 "attr_lookup"
            {"fall_time_after_threshold",
             LIBERTY_ATTRENUM_fall_time_after_threshold},
#line 189 "attr_lookup"
            {"variable_3", LIBERTY_ATTRENUM_variable_3},
#line 306 "attr_lookup"
            {"members", LIBERTY_ATTRENUM_members},
#line 87 "attr_lookup"
            {"level_shifter_enable_pin",
             LIBERTY_ATTRENUM_level_shifter_enable_pin},
#line 66 "attr_lookup"
            {"related_ground_pin", LIBERTY_ATTRENUM_related_ground_pin},
#line 205 "attr_lookup"
            {"variable_2", LIBERTY_ATTRENUM_variable_2},
#line 463 "attr_lookup"
            {"k_temp_wire_res", LIBERTY_ATTRENUM_k_temp_wire_res},
#line 277 "attr_lookup"
            {"default_timing", LIBERTY_ATTRENUM_default_timing},
#line 464 "attr_lookup"
            {"k_temp_wire_cap", LIBERTY_ATTRENUM_k_temp_wire_cap},
#line 76 "attr_lookup"
            {"threshold", LIBERTY_ATTRENUM_threshold},
#line 202 "attr_lookup"
            {"vimax", LIBERTY_ATTRENUM_vimax},
#line 500 "attr_lookup"
            {"k_process_recovery_rise",
             LIBERTY_ATTRENUM_k_process_recovery_rise},
#line 501 "attr_lookup"
            {"k_process_recovery_fall",
             LIBERTY_ATTRENUM_k_process_recovery_fall},
#line 215 "attr_lookup"
            {"downto", LIBERTY_ATTRENUM_downto},
#line 312 "attr_lookup"
            {"slew_control", LIBERTY_ATTRENUM_slew_control},
#line 411 "attr_lookup"
            {"voltage_unit", LIBERTY_ATTRENUM_voltage_unit},
#line 323 "attr_lookup"
            {"pin_func_type", LIBERTY_ATTRENUM_pin_func_type},
#line 191 "attr_lookup"
            {"vomax", LIBERTY_ATTRENUM_vomax},
#line 138 "attr_lookup"
            {"k_volt_rise_delay_intercept",
             LIBERTY_ATTRENUM_k_volt_rise_delay_intercept},
#line 230 "attr_lookup"
            {"row_address", LIBERTY_ATTRENUM_row_address},
#line 173 "attr_lookup"
            {"default_hold_coefficient",
             LIBERTY_ATTRENUM_default_hold_coefficient},
#line 344 "attr_lookup"
            {"hysteresis", LIBERTY_ATTRENUM_hysteresis},
#line 505 "attr_lookup"
            {"k_process_nochange_rise",
             LIBERTY_ATTRENUM_k_process_nochange_rise},
#line 506 "attr_lookup"
            {"k_process_nochange_fall",
             LIBERTY_ATTRENUM_k_process_nochange_fall},
#line 473 "attr_lookup"
            {"k_temp_recovery_rise", LIBERTY_ATTRENUM_k_temp_recovery_rise},
#line 103 "attr_lookup"
            {"pg_type", LIBERTY_ATTRENUM_pg_type},
#line 474 "attr_lookup"
            {"k_temp_recovery_fall", LIBERTY_ATTRENUM_k_temp_recovery_fall},
#line 427 "attr_lookup"
            {"nom_voltage", LIBERTY_ATTRENUM_nom_voltage},
#line 37 "attr_lookup"
            {"wave_rise", LIBERTY_ATTRENUM_wave_rise},
#line 393 "attr_lookup"
            {"fpga_family", LIBERTY_ATTRENUM_fpga_family},
#line 436 "attr_lookup"
            {"k_volt_wire_res", LIBERTY_ATTRENUM_k_volt_wire_res},
#line 540 "attr_lookup"
            {"default_operating_conditions",
             LIBERTY_ATTRENUM_default_operating_conditions},
#line 424 "attr_lookup"
            {"preferred_input_pad_voltage",
             LIBERTY_ATTRENUM_preferred_input_pad_voltage},
#line 490 "attr_lookup"
            {"k_process_wire_res", LIBERTY_ATTRENUM_k_process_wire_res},
#line 437 "attr_lookup"
            {"k_volt_wire_cap", LIBERTY_ATTRENUM_k_volt_wire_cap},
#line 128 "attr_lookup"
            {"power_model", LIBERTY_ATTRENUM_power_model},
#line 491 "attr_lookup"
            {"k_process_wire_cap", LIBERTY_ATTRENUM_k_process_wire_cap},
#line 67 "attr_lookup"
            {"related_power_pin", LIBERTY_ATTRENUM_related_power_pin},
#line 70 "attr_lookup"
            {"power_level", LIBERTY_ATTRENUM_power_level},
#line 383 "attr_lookup"
            {"mpm_libname", LIBERTY_ATTRENUM_mpm_libname},
#line 101 "attr_lookup"
            {"pg_function", LIBERTY_ATTRENUM_pg_function},
#line 16 "attr_lookup"
            {"curve_y", LIBERTY_ATTRENUM_curve_y},
#line 512 "attr_lookup"
            {"k_process_internal_power",
             LIBERTY_ATTRENUM_k_process_internal_power},
#line 80 "attr_lookup"
            {"pull_up_function", LIBERTY_ATTRENUM_pull_up_function},
#line 521 "attr_lookup"
            {"in_place_swap_mode", LIBERTY_ATTRENUM_in_place_swap_mode},
#line 381 "attr_lookup"
            {"observe_node", LIBERTY_ATTRENUM_observe_node},
#line 476 "attr_lookup"
            {"k_temp_nochange_rise", LIBERTY_ATTRENUM_k_temp_nochange_rise},
#line 477 "attr_lookup"
            {"k_temp_nochange_fall", LIBERTY_ATTRENUM_k_temp_nochange_fall},
#line 151 "attr_lookup"
            {"k_temp_fall_propagation",
             LIBERTY_ATTRENUM_k_temp_fall_propagation},
#line 446 "attr_lookup"
            {"k_volt_recovery_rise", LIBERTY_ATTRENUM_k_volt_recovery_rise},
#line 387 "attr_lookup"
            {"handle_negative_constraint",
             LIBERTY_ATTRENUM_handle_negative_constraint},
#line 181 "attr_lookup"
            {"fanout_area", LIBERTY_ATTRENUM_fanout_area},
#line 447 "attr_lookup"
            {"k_volt_recovery_fall", LIBERTY_ATTRENUM_k_volt_recovery_fall},
#line 354 "attr_lookup"
            {"driver_type", LIBERTY_ATTRENUM_driver_type},
#line 36 "attr_lookup"
            {"wave_fall", LIBERTY_ATTRENUM_wave_fall},
#line 320 "attr_lookup"
            {"pulling_current", LIBERTY_ATTRENUM_pulling_current},
#line 494 "attr_lookup"
            {"k_process_skew_rise", LIBERTY_ATTRENUM_k_process_skew_rise},
#line 495 "attr_lookup"
            {"k_process_skew_fall", LIBERTY_ATTRENUM_k_process_skew_fall},
#line 342 "attr_lookup"
            {"input_signal_level", LIBERTY_ATTRENUM_input_signal_level},
#line 395 "attr_lookup"
            {"fpga_cell_type", LIBERTY_ATTRENUM_fpga_cell_type},
#line 336 "attr_lookup"
            {"max_fanout", LIBERTY_ATTRENUM_max_fanout},
#line 539 "attr_lookup"
            {"default_output_pin_cap", LIBERTY_ATTRENUM_default_output_pin_cap},
#line 421 "attr_lookup"
            {"pulling_resistance_unit",
             LIBERTY_ATTRENUM_pulling_resistance_unit},
#line 537 "attr_lookup"
            {"default_output_pin_rise_res",
             LIBERTY_ATTRENUM_default_output_pin_rise_res},
#line 536 "attr_lookup"
            {"default_rise_delay_intercept",
             LIBERTY_ATTRENUM_default_rise_delay_intercept},
#line 538 "attr_lookup"
            {"default_output_pin_fall_res",
             LIBERTY_ATTRENUM_default_output_pin_fall_res},
#line 555 "attr_lookup"
            {"default_fall_delay_intercept",
             LIBERTY_ATTRENUM_default_fall_delay_intercept},
#line 343 "attr_lookup"
            {"input_map", LIBERTY_ATTRENUM_input_map},
#line 449 "attr_lookup"
            {"k_volt_nochange_rise", LIBERTY_ATTRENUM_k_volt_nochange_rise},
#line 450 "attr_lookup"
            {"k_volt_nochange_fall", LIBERTY_ATTRENUM_k_volt_nochange_fall},
#line 373 "attr_lookup"
            {"use_for_size_only", LIBERTY_ATTRENUM_use_for_size_only},
#line 140 "attr_lookup"
            {"k_volt_fall_propagation",
             LIBERTY_ATTRENUM_k_volt_fall_propagation},
#line 57 "attr_lookup"
            {"high", LIBERTY_ATTRENUM_high},
#line 467 "attr_lookup"
            {"k_temp_skew_rise", LIBERTY_ATTRENUM_k_temp_skew_rise},
#line 468 "attr_lookup"
            {"k_temp_skew_fall", LIBERTY_ATTRENUM_k_temp_skew_fall},
#line 17 "attr_lookup"
            {"curve_x", LIBERTY_ATTRENUM_curve_x},
#line 111 "attr_lookup"
            {"driver_waveform", LIBERTY_ATTRENUM_driver_waveform},
#line 305 "attr_lookup"
            {"related_bus_pins", LIBERTY_ATTRENUM_related_bus_pins},
#line 132 "attr_lookup"
            {"lsi_rounding_cutoff", LIBERTY_ATTRENUM_lsi_rounding_cutoff},
#line 114 "attr_lookup"
            {"timing_model_type", LIBERTY_ATTRENUM_timing_model_type},
#line 281 "attr_lookup"
            {"constraint_low", LIBERTY_ATTRENUM_constraint_low},
#line 110 "attr_lookup"
            {"driver_waveform_rise", LIBERTY_ATTRENUM_driver_waveform_rise},
#line 109 "attr_lookup"
            {"driver_waveform_fall", LIBERTY_ATTRENUM_driver_waveform_fall},
#line 541 "attr_lookup"
            {"default_min_porosity", LIBERTY_ATTRENUM_default_min_porosity},
#line 325 "attr_lookup"
            {"output_signal_level", LIBERTY_ATTRENUM_output_signal_level},
#line 183 "attr_lookup"
            {"fanout_length", LIBERTY_ATTRENUM_fanout_length},
#line 286 "attr_lookup"
            {"when", LIBERTY_ATTRENUM_when},
#line 370 "attr_lookup"
            {"xnf_schnm", LIBERTY_ATTRENUM_xnf_schnm},
#line 256 "attr_lookup"
            {"when_end", LIBERTY_ATTRENUM_when_end},
#line 440 "attr_lookup"
            {"k_volt_skew_rise", LIBERTY_ATTRENUM_k_volt_skew_rise},
#line 369 "attr_lookup"
            {"pin_equal", LIBERTY_ATTRENUM_pin_equal},
#line 441 "attr_lookup"
            {"k_volt_skew_fall", LIBERTY_ATTRENUM_k_volt_skew_fall},
#line 25 "attr_lookup"
            {"index_output", LIBERTY_ATTRENUM_index_output},
#line 253 "attr_lookup"
            {"edge_type", LIBERTY_ATTRENUM_edge_type},
#line 317 "attr_lookup"
            {"rise_current_slope_after_threshold",
             LIBERTY_ATTRENUM_rise_current_slope_after_threshold},
#line 231 "attr_lookup"
            {"column_address", LIBERTY_ATTRENUM_column_address},
#line 352 "attr_lookup"
            {"fall_current_slope_after_threshold",
             LIBERTY_ATTRENUM_fall_current_slope_after_threshold},
#line 131 "attr_lookup"
            {"lsi_rounding_digit", LIBERTY_ATTRENUM_lsi_rounding_digit},
#line 107 "attr_lookup"
            {"input_voltage_range", LIBERTY_ATTRENUM_input_voltage_range},
#line 371 "attr_lookup"
            {"xnf_device_name", LIBERTY_ATTRENUM_xnf_device_name},
#line 72 "attr_lookup"
            {"height_coefficient", LIBERTY_ATTRENUM_height_coefficient},
#line 6 "attr_lookup"
            {"driver_waveform_name", LIBERTY_ATTRENUM_driver_waveform_name},
#line 153 "attr_lookup"
            {"k_temp_fall_delay_intercept",
             LIBERTY_ATTRENUM_k_temp_fall_delay_intercept},
#line 296 "attr_lookup"
            {"equal_or_opposite_output",
             LIBERTY_ATTRENUM_equal_or_opposite_output},
#line 209 "attr_lookup"
            {"variable_5_range", LIBERTY_ATTRENUM_variable_5_range},
#line 545 "attr_lookup"
            {"default_max_capacitance",
             LIBERTY_ATTRENUM_default_max_capacitance},
#line 210 "attr_lookup"
            {"variable_4_range", LIBERTY_ATTRENUM_variable_4_range},
#line 313 "attr_lookup"
            {"signal_type", LIBERTY_ATTRENUM_signal_type},
#line 543 "attr_lookup"
            {"default_max_transition", LIBERTY_ATTRENUM_default_max_transition},
#line 434 "attr_lookup"
            {"key_feature", LIBERTY_ATTRENUM_key_feature},
#line 75 "attr_lookup"
            {"variable_7_range", LIBERTY_ATTRENUM_variable_7_range},
#line 360 "attr_lookup"
            {"clock_gate_test_pin", LIBERTY_ATTRENUM_clock_gate_test_pin},
#line 213 "attr_lookup"
            {"variable_1_range", LIBERTY_ATTRENUM_variable_1_range},
#line 211 "attr_lookup"
            {"variable_3_range", LIBERTY_ATTRENUM_variable_3_range},
#line 308 "attr_lookup"
            {"x_function", LIBERTY_ATTRENUM_x_function},
#line 208 "attr_lookup"
            {"variable_6_range", LIBERTY_ATTRENUM_variable_6_range},
#line 71 "attr_lookup"
            {"width_coefficient", LIBERTY_ATTRENUM_width_coefficient},
#line 186 "attr_lookup"
            {"slower_factor", LIBERTY_ATTRENUM_slower_factor},
#line 212 "attr_lookup"
            {"variable_2_range", LIBERTY_ATTRENUM_variable_2_range},
#line 188 "attr_lookup"
            {"default_power_rail", LIBERTY_ATTRENUM_default_power_rail},
#line 483 "attr_lookup"
            {"k_temp_internal_power", LIBERTY_ATTRENUM_k_temp_internal_power},
#line 422 "attr_lookup"
            {"preferred_output_pad_voltage",
             LIBERTY_ATTRENUM_preferred_output_pad_voltage},
#line 142 "attr_lookup"
            {"k_volt_fall_delay_intercept",
             LIBERTY_ATTRENUM_k_volt_fall_delay_intercept},
#line 532 "attr_lookup"
            {"default_wire_load", LIBERTY_ATTRENUM_default_wire_load},
#line 531 "attr_lookup"
            {"default_wire_load_area", LIBERTY_ATTRENUM_default_wire_load_area},
#line 257 "attr_lookup"
            {"timing_type", LIBERTY_ATTRENUM_timing_type},
#line 21 "attr_lookup"
            {"related_switch_pin", LIBERTY_ATTRENUM_related_switch_pin},
#line 389 "attr_lookup"
            {"fpga_min_degen_input_size",
             LIBERTY_ATTRENUM_fpga_min_degen_input_size},
#line 528 "attr_lookup"
            {"default_wire_load_resistance",
             LIBERTY_ATTRENUM_default_wire_load_resistance},
#line 407 "attr_lookup"
            {"routing_layers", LIBERTY_ATTRENUM_routing_layers},
#line 406 "attr_lookup"
            {"technology", LIBERTY_ATTRENUM_technology},
#line 275 "attr_lookup"
            {"fpga_timing_type", LIBERTY_ATTRENUM_fpga_timing_type},
#line 321 "attr_lookup"
            {"primary_output", LIBERTY_ATTRENUM_primary_output},
#line 435 "attr_lookup"
            {"key_bit", LIBERTY_ATTRENUM_key_bit},
#line 530 "attr_lookup"
            {"default_wire_load_capacitance",
             LIBERTY_ATTRENUM_default_wire_load_capacitance},
#line 456 "attr_lookup"
            {"k_volt_internal_power", LIBERTY_ATTRENUM_k_volt_internal_power},
#line 364 "attr_lookup"
            {"clock_gate_clock_pin", LIBERTY_ATTRENUM_clock_gate_clock_pin},
#line 527 "attr_lookup"
            {"default_wire_load_selection",
             LIBERTY_ATTRENUM_default_wire_load_selection},
#line 232 "attr_lookup"
            {"address_width", LIBERTY_ATTRENUM_address_width},
#line 246 "attr_lookup"
            {"next_state", LIBERTY_ATTRENUM_next_state},
#line 79 "attr_lookup"
            {"pull_down_function", LIBERTY_ATTRENUM_pull_down_function},
#line 65 "attr_lookup"
            {"switch_pin", LIBERTY_ATTRENUM_switch_pin},
#line 282 "attr_lookup"
            {"constraint_high", LIBERTY_ATTRENUM_constraint_high},
#line 235 "attr_lookup"
            {"enable_also", LIBERTY_ATTRENUM_enable_also},
#line 346 "attr_lookup"
            {"fpga_degenerate_output", LIBERTY_ATTRENUM_fpga_degenerate_output},
#line 46 "attr_lookup"
            {"edge_rate_sensitivity_r1",
             LIBERTY_ATTRENUM_edge_rate_sensitivity_r1},
#line 220 "attr_lookup"
            {"base_type", LIBERTY_ATTRENUM_base_type},
#line 307 "attr_lookup"
            {"input_map_shift", LIBERTY_ATTRENUM_input_map_shift},
#line 48 "attr_lookup"
            {"edge_rate_sensitivity_f1",
             LIBERTY_ATTRENUM_edge_rate_sensitivity_f1},
#line 47 "attr_lookup"
            {"edge_rate_sensitivity_r0",
             LIBERTY_ATTRENUM_edge_rate_sensitivity_r0},
#line 529 "attr_lookup"
            {"default_wire_load_mode", LIBERTY_ATTRENUM_default_wire_load_mode},
#line 518 "attr_lookup"
            {"k_process_cell_leakage_power",
             LIBERTY_ATTRENUM_k_process_cell_leakage_power},
#line 49 "attr_lookup"
            {"edge_rate_sensitivity_f0",
             LIBERTY_ATTRENUM_edge_rate_sensitivity_f0},
#line 264 "attr_lookup"
            {"steady_state_resistance_float_max",
             LIBERTY_ATTRENUM_steady_state_resistance_float_max},
#line 337 "attr_lookup"
            {"max_capacitance", LIBERTY_ATTRENUM_max_capacitance},
#line 92 "attr_lookup"
            {"has_builtin_pad", LIBERTY_ATTRENUM_has_builtin_pad},
#line 84 "attr_lookup"
            {"min_input_noise_width", LIBERTY_ATTRENUM_min_input_noise_width},
#line 39 "attr_lookup"
            {"steady_state_resistance_low",
             LIBERTY_ATTRENUM_steady_state_resistance_low},
#line 242 "attr_lookup"
            {"duty_cycle", LIBERTY_ATTRENUM_duty_cycle},
#line 310 "attr_lookup"
            {"test_output_only", LIBERTY_ATTRENUM_test_output_only},
#line 544 "attr_lookup"
            {"default_max_fanout", LIBERTY_ATTRENUM_default_max_fanout},
#line 194 "attr_lookup"
            {"power_rail", LIBERTY_ATTRENUM_power_rail},
#line 394 "attr_lookup"
            {"fpga_complex_degenerate",
             LIBERTY_ATTRENUM_fpga_complex_degenerate},
#line 23 "attr_lookup"
            {"input_low_value", LIBERTY_ATTRENUM_input_low_value},
#line 326 "attr_lookup"
            {"nextstate_type", LIBERTY_ATTRENUM_nextstate_type},
#line 361 "attr_lookup"
            {"clock_gate_out_pin", LIBERTY_ATTRENUM_clock_gate_out_pin},
#line 522 "attr_lookup"
            {"fpga_prefer_undegenerated_gates",
             LIBERTY_ATTRENUM_fpga_prefer_undegenerated_gates},
#line 251 "attr_lookup"
            {"bus_type", LIBERTY_ATTRENUM_bus_type},
#line 489 "attr_lookup"
            {"k_temp_cell_leakage_power",
             LIBERTY_ATTRENUM_k_temp_cell_leakage_power},
#line 97 "attr_lookup"
            {"edge_rate_breakpoint_r1",
             LIBERTY_ATTRENUM_edge_rate_breakpoint_r1},
#line 99 "attr_lookup"
            {"edge_rate_breakpoint_f1",
             LIBERTY_ATTRENUM_edge_rate_breakpoint_f1},
#line 314 "attr_lookup"
            {"rise_time_before_threshold",
             LIBERTY_ATTRENUM_rise_time_before_threshold},
#line 349 "attr_lookup"
            {"fall_time_before_threshold",
             LIBERTY_ATTRENUM_fall_time_before_threshold},
#line 98 "attr_lookup"
            {"edge_rate_breakpoint_r0",
             LIBERTY_ATTRENUM_edge_rate_breakpoint_r0},
#line 100 "attr_lookup"
            {"edge_rate_breakpoint_f0",
             LIBERTY_ATTRENUM_edge_rate_breakpoint_f0},
#line 391 "attr_lookup"
            {"fpga_lut_output", LIBERTY_ATTRENUM_fpga_lut_output},
#line 243 "attr_lookup"
            {"divided_by", LIBERTY_ATTRENUM_divided_by},
#line 404 "attr_lookup"
            {"auxiliary_pad_cell", LIBERTY_ATTRENUM_auxiliary_pad_cell},
#line 259 "attr_lookup"
            {"steady_state_resistance_low_min",
             LIBERTY_ATTRENUM_steady_state_resistance_low_min},
#line 542 "attr_lookup"
            {"default_max_utilization",
             LIBERTY_ATTRENUM_default_max_utilization},
#line 462 "attr_lookup"
            {"k_volt_cell_leakage_power",
             LIBERTY_ATTRENUM_k_volt_cell_leakage_power},
#line 408 "attr_lookup"
            {"library_features", LIBERTY_ATTRENUM_library_features},
#line 423 "attr_lookup"
            {"preferred_output_pad_slew_rate_control",
             LIBERTY_ATTRENUM_preferred_output_pad_slew_rate_control},
#line 430 "attr_lookup"
            {"leakage_power_unit", LIBERTY_ATTRENUM_leakage_power_unit},
#line 174 "attr_lookup"
            {"default_edge_rate_breakpoint_r1",
             LIBERTY_ATTRENUM_default_edge_rate_breakpoint_r1},
#line 19 "attr_lookup"
            {"base_curve_type", LIBERTY_ATTRENUM_base_curve_type},
#line 176 "attr_lookup"
            {"default_edge_rate_breakpoint_f1",
             LIBERTY_ATTRENUM_default_edge_rate_breakpoint_f1},
#line 182 "attr_lookup"
            {"wire_load_from_area", LIBERTY_ATTRENUM_wire_load_from_area},
#line 175 "attr_lookup"
            {"default_edge_rate_breakpoint_r0",
             LIBERTY_ATTRENUM_default_edge_rate_breakpoint_r0},
#line 177 "attr_lookup"
            {"default_edge_rate_breakpoint_f0",
             LIBERTY_ATTRENUM_default_edge_rate_breakpoint_f0},
#line 288 "attr_lookup"
            {"switching_interval", LIBERTY_ATTRENUM_switching_interval},
#line 272 "attr_lookup"
            {"related_bus_equivalent", LIBERTY_ATTRENUM_related_bus_equivalent},
#line 425 "attr_lookup"
            {"output_threshold_pct_rise",
             LIBERTY_ATTRENUM_output_threshold_pct_rise},
#line 557 "attr_lookup"
            {"default_cell_leakage_power",
             LIBERTY_ATTRENUM_default_cell_leakage_power},
#line 426 "attr_lookup"
            {"output_threshold_pct_fall",
             LIBERTY_ATTRENUM_output_threshold_pct_fall},
#line 40 "attr_lookup"
            {"steady_state_resistance_high",
             LIBERTY_ATTRENUM_steady_state_resistance_high},
#line 116 "attr_lookup"
            {"switch_cell_type", LIBERTY_ATTRENUM_switch_cell_type},
#line 13 "attr_lookup"
            {"base_curves_group", LIBERTY_ATTRENUM_base_curves_group},
#line 324 "attr_lookup"
            {"output_voltage", LIBERTY_ATTRENUM_output_voltage},
#line 27 "attr_lookup"
            {"input_switching_condition",
             LIBERTY_ATTRENUM_input_switching_condition},
#line 390 "attr_lookup"
            {"fpga_max_degen_input_size",
             LIBERTY_ATTRENUM_fpga_max_degen_input_size},
#line 261 "attr_lookup"
            {"steady_state_resistance_high_min",
             LIBERTY_ATTRENUM_steady_state_resistance_high_min},
#line 255 "attr_lookup"
            {"when_start", LIBERTY_ATTRENUM_when_start},
#line 102 "attr_lookup"
            {"switch_function", LIBERTY_ATTRENUM_switch_function},
#line 62 "attr_lookup"
            {"input_signal_level_low", LIBERTY_ATTRENUM_input_signal_level_low},
#line 316 "attr_lookup"
            {"rise_current_slope_before_threshold",
             LIBERTY_ATTRENUM_rise_current_slope_before_threshold},
#line 351 "attr_lookup"
            {"fall_current_slope_before_threshold",
             LIBERTY_ATTRENUM_fall_current_slope_before_threshold},
#line 239 "attr_lookup"
            {"multiplied_by", LIBERTY_ATTRENUM_multiplied_by},
#line 363 "attr_lookup"
            {"clock_gate_enable_pin", LIBERTY_ATTRENUM_clock_gate_enable_pin},
#line 295 "attr_lookup"
            {"falling_together_group", LIBERTY_ATTRENUM_falling_together_group},
#line 362 "attr_lookup"
            {"clock_gate_obs_pin", LIBERTY_ATTRENUM_clock_gate_obs_pin},
#line 50 "attr_lookup"
            {"clock_gating_flag", LIBERTY_ATTRENUM_clock_gating_flag},
#line 82 "attr_lookup"
            {"output_signal_level_low",
             LIBERTY_ATTRENUM_output_signal_level_low},
#line 402 "attr_lookup"
            {"cell_leakage_power", LIBERTY_ATTRENUM_cell_leakage_power},
#line 396 "attr_lookup"
            {"fpga_bridge_inputs", LIBERTY_ATTRENUM_fpga_bridge_inputs},
#line 64 "attr_lookup"
            {"power_down_function", LIBERTY_ATTRENUM_power_down_function},
#line 401 "attr_lookup"
            {"clock_gating_integrated_cell",
             LIBERTY_ATTRENUM_clock_gating_integrated_cell},
#line 228 "attr_lookup"
            {"word_width", LIBERTY_ATTRENUM_word_width},
#line 22 "attr_lookup"
            {"input_high_value", LIBERTY_ATTRENUM_input_high_value},
#line 289 "attr_lookup"
            {"rising_together_group", LIBERTY_ATTRENUM_rising_together_group},
#line 115 "attr_lookup"
            {"threshold_voltage_group",
             LIBERTY_ATTRENUM_threshold_voltage_group},
#line 414 "attr_lookup"
            {"slew_upper_threshold_pct_rise",
             LIBERTY_ATTRENUM_slew_upper_threshold_pct_rise},
#line 415 "attr_lookup"
            {"slew_upper_threshold_pct_fall",
             LIBERTY_ATTRENUM_slew_upper_threshold_pct_fall},
#line 63 "attr_lookup"
            {"input_signal_level_high",
             LIBERTY_ATTRENUM_input_signal_level_high},
#line 85 "attr_lookup"
            {"max_input_noise_width", LIBERTY_ATTRENUM_max_input_noise_width},
#line 217 "attr_lookup"
            {"bit_width", LIBERTY_ATTRENUM_bit_width},
#line 392 "attr_lookup"
            {"fpga_lut_insert_before_sequential",
             LIBERTY_ATTRENUM_fpga_lut_insert_before_sequential},
#line 106 "attr_lookup"
            {"output_voltage_range", LIBERTY_ATTRENUM_output_voltage_range},
#line 418 "attr_lookup"
            {"slew_derate_from_library",
             LIBERTY_ATTRENUM_slew_derate_from_library},
#line 561 "attr_lookup"
            {"bus_naming_style", LIBERTY_ATTRENUM_bus_naming_style},
#line 83 "attr_lookup"
            {"output_signal_level_high",
             LIBERTY_ATTRENUM_output_signal_level_high},
#line 260 "attr_lookup"
            {"steady_state_resistance_low_max",
             LIBERTY_ATTRENUM_steady_state_resistance_low_max},
#line 78 "attr_lookup"
            {"power_gating_pin", LIBERTY_ATTRENUM_power_gating_pin},
#line 118 "attr_lookup"
            {"power_gating_cell", LIBERTY_ATTRENUM_power_gating_cell},
#line 26 "attr_lookup"
            {"output_switching_condition",
             LIBERTY_ATTRENUM_output_switching_condition},
#line 546 "attr_lookup"
            {"default_leakage_power_density",
             LIBERTY_ATTRENUM_default_leakage_power_density},
#line 416 "attr_lookup"
            {"slew_lower_threshold_pct_rise",
             LIBERTY_ATTRENUM_slew_lower_threshold_pct_rise},
#line 417 "attr_lookup"
            {"slew_lower_threshold_pct_fall",
             LIBERTY_ATTRENUM_slew_lower_threshold_pct_fall},
#line 335 "attr_lookup"
            {"max_time_borrow", LIBERTY_ATTRENUM_max_time_borrow},
#line 329 "attr_lookup"
            {"min_pulse_width_low", LIBERTY_ATTRENUM_min_pulse_width_low},
#line 168 "attr_lookup"
            {"default_threshold_voltage_group",
             LIBERTY_ATTRENUM_default_threshold_voltage_group},
#line 523 "attr_lookup"
            {"fpga_allow_duplicate_degenerated_gates",
             LIBERTY_ATTRENUM_fpga_allow_duplicate_degenerated_gates},
#line 262 "attr_lookup"
            {"steady_state_resistance_high_max",
             LIBERTY_ATTRENUM_steady_state_resistance_high_max},
#line 478 "attr_lookup"
            {"k_temp_min_pulse_width_low",
             LIBERTY_ATTRENUM_k_temp_min_pulse_width_low},
#line 507 "attr_lookup"
            {"k_process_min_pulse_width_low",
             LIBERTY_ATTRENUM_k_process_min_pulse_width_low},
#line 42 "attr_lookup"
            {"steady_state_resistance_above_high",
             LIBERTY_ATTRENUM_steady_state_resistance_above_high},
#line 451 "attr_lookup"
            {"k_volt_min_pulse_width_low",
             LIBERTY_ATTRENUM_k_volt_min_pulse_width_low},
#line 330 "attr_lookup"
            {"min_pulse_width_high", LIBERTY_ATTRENUM_min_pulse_width_high},
#line 122 "attr_lookup"
            {"ignore_verify_icg_type", LIBERTY_ATTRENUM_ignore_verify_icg_type},
#line 479 "attr_lookup"
            {"k_temp_min_pulse_width_high",
             LIBERTY_ATTRENUM_k_temp_min_pulse_width_high},
#line 508 "attr_lookup"
            {"k_process_min_pulse_width_high",
             LIBERTY_ATTRENUM_k_process_min_pulse_width_high},
#line 452 "attr_lookup"
            {"k_volt_min_pulse_width_high",
             LIBERTY_ATTRENUM_k_volt_min_pulse_width_high},
#line 41 "attr_lookup"
            {"steady_state_resistance_below_low",
             LIBERTY_ATTRENUM_steady_state_resistance_below_low},
#line 287 "attr_lookup"
            {"switching_together_group",
             LIBERTY_ATTRENUM_switching_together_group}};

        static const short lookup[] = {
            -1,  -1,  -1,  -1,  0,   -1,  -1,  -1,  -1,  -1,  -1,  1,   -1,
            -1,  -1,  2,   -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  3,   -1,  -1,  -1,  4,   -1,  -1,  -1,  -1,  -1,
            -1,  5,   -1,  -1,  -1,  6,   7,   -1,  -1,  -1,  -1,  8,   -1,
            9,   -1,  -1,  10,  -1,  -1,  -1,  11,  12,  -1,  13,  -1,  -1,
            -1,  14,  15,  -1,  16,  -1,  -1,  17,  -1,  18,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  19,  -1,  -1,  -1,  -1,  20,  -1,
            21,  22,  -1,  23,  -1,  -1,  -1,  -1,  -1,  -1,  24,  -1,  -1,
            -1,  -1,  -1,  25,  26,  -1,  -1,  -1,  -1,  27,  -1,  -1,  -1,
            -1,  28,  -1,  -1,  29,  -1,  30,  31,  32,  33,  -1,  -1,  34,
            35,  -1,  -1,  36,  -1,  37,  -1,  -1,  -1,  -1,  -1,  38,  -1,
            -1,  39,  40,  -1,  -1,  41,  -1,  42,  -1,  -1,  -1,  -1,  43,
            -1,  -1,  44,  -1,  -1,  -1,  -1,  -1,  -1,  45,  46,  -1,  47,
            -1,  48,  -1,  49,  -1,  -1,  -1,  50,  51,  -1,  52,  -1,  -1,
            53,  54,  -1,  55,  -1,  -1,  -1,  56,  57,  -1,  -1,  58,  59,
            60,  -1,  -1,  -1,  -1,  -1,  61,  -1,  62,  -1,  -1,  -1,  -1,
            -1,  63,  -1,  -1,  64,  -1,  65,  -1,  -1,  -1,  66,  67,  -1,
            68,  -1,  -1,  69,  -1,  -1,  70,  -1,  71,  -1,  -1,  72,  -1,
            -1,  -1,  -1,  73,  -1,  74,  75,  -1,  -1,  76,  77,  78,  79,
            80,  -1,  -1,  -1,  -1,  -1,  -1,  81,  82,  -1,  -1,  83,  -1,
            84,  -1,  85,  -1,  -1,  -1,  -1,  86,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  87,  -1,  -1,  -1,  -1,  -1,  -1,  88,  -1,  -1,  89,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  90,  -1,  -1,
            -1,  -1,  91,  92,  -1,  -1,  -1,  93,  94,  -1,  -1,  95,  96,
            97,  -1,  -1,  -1,  98,  -1,  -1,  -1,  -1,  -1,  -1,  99,  100,
            -1,  -1,  -1,  -1,  101, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            102, -1,  103, -1,  -1,  -1,  -1,  -1,  104, 105, -1,  -1,  106,
            107, -1,  -1,  -1,  108, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  109, -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  110, -1,  -1,  -1,  111, -1,  112, -1,  -1,  -1,  -1,  113,
            -1,  -1,  114, 115, 116, -1,  -1,  -1,  117, -1,  -1,  -1,  -1,
            118, 119, 120, -1,  121, -1,  -1,  122, -1,  123, 124, 125, -1,
            -1,  126, -1,  127, 128, -1,  -1,  -1,  129, -1,  -1,  -1,  130,
            131, -1,  -1,  -1,  132, -1,  133, -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  134, -1,  135, 136, -1,  -1,  -1,
            -1,  -1,  -1,  -1,  137, 138, -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  139, -1,  -1,  -1,  -1,  140, -1,  -1,  -1,
            -1,  141, -1,  -1,  142, 143, -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  144, 145, -1,  146, -1,  -1,  -1,  -1,  147, -1,  -1,
            148, 149, 150, 151, 152, -1,  -1,  153, -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  154, -1,  155, -1,  156, -1,  -1,  157, -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  158, -1,
            -1,  159, -1,  160, -1,  -1,  161, 162, 163, 164, 165, -1,  166,
            167, 168, 169, -1,  -1,  170, 171, -1,  -1,  -1,  -1,  -1,  172,
            173, -1,  -1,  -1,  -1,  -1,  -1,  174, -1,  -1,  -1,  -1,  -1,
            -1,  175, -1,  -1,  176, -1,  177, -1,  178, -1,  -1,  -1,  -1,
            -1,  -1,  179, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  180, -1,
            -1,  -1,  -1,  181, 182, -1,  -1,  183, -1,  -1,  184, 185, -1,
            186, 187, -1,  -1,  -1,  188, -1,  189, -1,  -1,  190, 191, 192,
            -1,  193, -1,  -1,  -1,  -1,  -1,  -1,  194, 195, 196, 197, 198,
            -1,  199, -1,  200, 201, -1,  202, -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  203, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  204, -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  205, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  206, -1,  -1,  -1,  -1,  207, -1,  -1,  -1,  -1,  -1,
            -1,  208, -1,  -1,  -1,  -1,  209, -1,  -1,  -1,  -1,  210, -1,
            211, -1,  -1,  -1,  -1,  212, -1,  -1,  -1,  -1,  213, -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  214, -1,  215, -1,  -1,  -1,
            216, -1,  217, 218, -1,  219, -1,  -1,  -1,  -1,  220, -1,  -1,
            -1,  -1,  221, -1,  -1,  -1,  -1,  -1,  222, -1,  223, -1,  -1,
            -1,  -1,  224, -1,  -1,  225, -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  226, -1,  -1,  -1,  -1,  -1,  -1,  -1,  227, -1,  228,
            -1,  -1,  229, -1,  -1,  -1,  -1,  230, -1,  -1,  -1,  -1,  -1,
            231, -1,  -1,  232, -1,  -1,  -1,  233, -1,  -1,  -1,  -1,  -1,
            -1,  234, 235, -1,  -1,  -1,  236, -1,  -1,  -1,  237, -1,  -1,
            -1,  -1,  -1,  -1,  238, -1,  -1,  -1,  -1,  -1,  -1,  -1,  239,
            -1,  -1,  -1,  -1,  -1,  240, 241, -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  242, -1,  243, 244, -1,  245, 246, 247, -1,  -1,
            -1,  -1,  -1,  248, 249, 250, 251, 252, 253, -1,  254, -1,  255,
            -1,  256, 257, 258, 259, 260, 261, -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  262, -1,  -1,  263, -1,  -1,  -1,  264, -1,  -1,  -1,
            -1,  -1,  265, -1,  266, -1,  -1,  -1,  -1,  -1,  267, 268, -1,
            -1,  -1,  269, -1,  -1,  270, 271, 272, -1,  -1,  273, -1,  274,
            -1,  275, 276, -1,  -1,  -1,  -1,  -1,  277, -1,  -1,  -1,  -1,
            -1,  -1,  278, -1,  -1,  279, -1,  -1,  -1,  -1,  280, -1,  -1,
            -1,  281, -1,  -1,  282, 283, 284, -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  285, 286, -1,  287, -1,  288, -1,  -1,  289, -1,  -1,
            -1,  -1,  290, -1,  291, 292, 293, 294, -1,  -1,  -1,  -1,  -1,
            -1,  -1,  295, 296, -1,  -1,  -1,  -1,  297, 298, 299, -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  300, 301, -1,
            -1,  -1,  302, 303, -1,  304, -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  305, -1,  -1,  306, -1,  -1,  307, -1,  -1,
            -1,  308, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  309, 310, -1,
            -1,  -1,  311, 312, -1,  -1,  -1,  -1,  -1,  -1,  -1,  313, -1,
            -1,  -1,  -1,  314, -1,  -1,  315, -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  316, -1,  -1,  -1,  -1,  317, 318, -1,  319,
            -1,  320, -1,  -1,  -1,  321, -1,  -1,  -1,  -1,  -1,  -1,  -1,
            322, -1,  -1,  -1,  -1,  -1,  323, -1,  -1,  324, -1,  -1,  -1,
            -1,  325, -1,  326, -1,  327, -1,  -1,  328, 329, -1,  -1,  330,
            -1,  331, -1,  -1,  -1,  332, -1,  -1,  333, -1,  -1,  -1,  334,
            335, -1,  336, -1,  -1,  -1,  -1,  -1,  337, -1,  338, -1,  -1,
            -1,  339, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  340, -1,  -1,
            -1,  -1,  -1,  341, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  342, 343, -1,  344, -1,  345, -1,  346, -1,
            -1,  -1,  -1,  347, -1,  -1,  -1,  -1,  -1,  -1,  348, -1,  -1,
            -1,  -1,  349, -1,  -1,  350, -1,  351, 352, 353, -1,  -1,  354,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  355, -1,  -1,
            356, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  357, -1,
            -1,  -1,  358, -1,  -1,  -1,  -1,  359, -1,  -1,  -1,  360, 361,
            -1,  -1,  -1,  -1,  -1,  362, -1,  363, 364, -1,  -1,  -1,  365,
            366, -1,  -1,  -1,  367, 368, -1,  369, -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  370, -1,  -1,  -1,  -1,  371, -1,  372, 373, 374, -1,  375,
            -1,  -1,  -1,  -1,  376, -1,  -1,  -1,  -1,  -1,  377, -1,  -1,
            -1,  -1,  -1,  -1,  -1,  378, -1,  379, -1,  380, -1,  -1,  381,
            -1,  382, -1,  -1,  -1,  -1,  -1,  383, -1,  -1,  -1,  -1,  384,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  385, -1,  -1,  -1,  386, -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  387, 388,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  389, -1,  -1,  -1,
            390, -1,  -1,  391, -1,  -1,  -1,  392, 393, 394, -1,  395, -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  396, 397, -1,  -1,  -1,
            398, -1,  -1,  -1,  399, 400, 401, -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  402, -1,  403, -1,  404, -1,  -1,
            -1,  -1,  -1,  -1,  405, -1,  -1,  406, 407, -1,  -1,  -1,  408,
            -1,  -1,  -1,  409, 410, -1,  -1,  -1,  411, 412, -1,  -1,  413,
            -1,  414, -1,  -1,  -1,  -1,  415, -1,  -1,  416, -1,  417, -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  418, 419, -1,  -1,  -1,
            420, -1,  -1,  -1,  -1,  -1,  421, -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  422, -1,
            423, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  424, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  425, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  426, -1,  -1,  -1,  427, -1,  428, -1,
            429, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            430, -1,  -1,  -1,  -1,  -1,  431, 432, 433, -1,  -1,  434, -1,
            -1,  -1,  435, -1,  -1,  -1,  -1,  -1,  436, -1,  -1,  -1,  -1,
            -1,  -1,  437, -1,  -1,  -1,  -1,  438, 439, -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  440, -1,  -1,  441, -1,  442, -1,  443, -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  444, -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  445, -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  446, -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  447, 448, 449, -1,  -1,
            450, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  451, -1,  -1,
            452, 453, 454, -1,  -1,  -1,  455, -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  456, -1,  -1,  -1,  -1,  457, -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  458, 459, -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  460, 461, -1,  462,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  463, -1,  464, -1,  -1,  -1,  -1,  -1,  -1,
            465, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  466, -1,  -1,  -1,
            -1,  467, -1,  468, -1,  -1,  469, 470, -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  471, -1,  -1,  -1,  -1,  472, -1,  -1,  473, -1,
            -1,  -1,  -1,  474, -1,  475, -1,  -1,  -1,  -1,  476, -1,  -1,
            -1,  -1,  -1,  477, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  478, -1,  479, -1,  -1,  -1,  -1,  -1,  -1,  -1,  480, -1,
            481, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  482, -1,
            483, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  484, -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  485, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  486, -1,  -1,  -1,  487, 488,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  489, -1,  490, -1,  -1,  -1,
            -1,  491, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  492, -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  493, -1,  494, 495, -1,  -1,  -1,
            496, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  497, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  498, -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  499, 500, -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  501, -1,  -1,  -1,  -1,
            502, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  503, -1,  -1,  -1,  504, -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  505, -1,  -1,  -1,  -1,  -1,  -1,
            506, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  507, -1,  -1,  -1,
            -1,  508, -1,  -1,  509, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            510, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  511, -1,
            -1,  -1,  -1,  -1,  -1,  -1,  512, -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  513, -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  514, -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  515, -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  516, -1,  -1,  -1,  -1,  517, -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  518, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  519, -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  520, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            521, -1,  522, -1,  -1,  -1,  -1,  -1,  523, -1,  -1,  -1,  -1,
            524, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  525, -1,  -1,  526, -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  527,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  528, 529, -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  530, -1,  531, -1,  -1,  -1,  -1,  -1,  -1,
            532, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  533,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  534,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  535, -1,  -1,
            -1,  -1,  -1,  -1,  -1,  536, -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  537, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            538, -1,  -1,  -1,  -1,  539, -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  540, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  541, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  542,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  543,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  544, -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  545, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  546,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  547, 548, -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  549, -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            550, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  551, -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  552, -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            553, -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  554, -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
            -1,  -1,  555};

        if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
                register int key = attr_hash_func(str, len);

                if (key <= MAX_HASH_VALUE && key >= 0) {
                        register int index = lookup[key];

                        if (index >= 0) {
                                register const char *s = wordlist[index].name;

                                if (*str == *s && !strcmp(str + 1, s + 1))
                                        return &wordlist[index];
                        }
                }
        }
        return 0;
}
#line 562 "attr_lookup"
