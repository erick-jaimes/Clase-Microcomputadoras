# El código para las tablas de búsqueda ha sido sacado de:
# http://www.piclist.com/techref/microchip/tables.htm
# El código para reproducir la melodía es sacado de:
# https://dragaosemchama.com/en/2019/02/songs-for-arduino/
import glob
import os
import re

DO = 0
RE = 1
MI = 2
FA = 3
SOL = 4
LA = 5
SI = 6
REST = 7

#tiempo = 108
tiempo = 108
wholenote = (60_000 * 4) / tiempo

def remove_file_extension(filename):
    return filename.replace('.tonada.txt', '')


class CommaSeparatedDataSender():
    def __init__(self, dest_file):
        self._dest_file = dest_file
        self._first_data_already_printed = False
        self._line_length = 0
        self._transmited_bytes = 0


    # Start definition of table
    dt = '    DT '
    def table_directive(self):
        dt = '    DT '
        print(dt, end='', file=self._dest_file)
        self._line_length = len(dt)


    def send_data(self, data):
        if data[0] == '"':
            self._transmited_bytes += len(data) - 2
        else:
            self._transmited_bytes += 1

        if self._line_length + len(data) + len(', ') > 80:
            print('\n', end='', file=self._dest_file)
            self.table_directive()
            self._first_data_already_printed = False

        if self._first_data_already_printed:
            print(', ', end='', file=self._dest_file)
            self._line_length += len(', ')

        print(data, end='', file=self._dest_file)
        self._line_length += len(data)
        self._first_data_already_printed = True


def generate_table(index, str_filename, dest_file):
    first_data_printed = True

    # Name of the subroutine
    subroutine_name = remove_file_extension(str_filename)
    print(f'''
{subroutine_name}:
    bcf STATUS, RP1
    bcf STATUS, RP0 ; Banco 0

    addlw LOW(tonada{index}__table)
    movwf (TONADA__temp & 0x7F)
    rlf (TONADA__zero & 0x7F), W
    addlw HIGH(tonada{index}__table)
    movwf PCLATH
    movf (TONADA__temp & 0x7F), W
    movwf PCL
tonada{index}__table''', file=dest_file)

    sender = CommaSeparatedDataSender(dest_file)
    sender.table_directive()

    # Tamaño del título y el texto
    sender.send_data(hex(len(subroutine_name)).upper())
    sender.send_data(f'"{subroutine_name}"')

    # Notas

    with open(str_filename, 'r')  as str_file:

        lines = str_file.readlines()
        bytes_needed = len(lines) * 2
        sender.send_data(hex(bytes_needed).upper())

        print("Bytes de EEPROM que se usarán: ", len(subroutine_name) + bytes_needed + 2)

        for line in lines:
            note, divider = line.split(',')

            if re.match(r'REST', note):
                note = REST
            elif re.match(r'NOTE_C.*', note) or re.match(r'NOTE_DO.*', note):
                note = DO
            elif re.match(r'NOTE_D.*', note) or re.match(r'NOTE_RE.*', note):
                note = RE
            elif re.match(r'NOTE_E.*', note) or re.match(r'NOTE_MI.*', note):
                note = MI
            elif re.match(r'NOTE_F.*', note) or re.match(r'NOTE_FA.*', note):
                note = FA
            elif re.match(r'NOTE_G.*', note) or re.match(r'NOTE_SOL.*', note):
                note = SOL
            elif re.match(r'NOTE_A.*', note) or re.match(r'NOTE_LA.*', note):
                note = LA
            elif re.match(r'NOTE_B.*', note) or re.match(r'NOTE_SI.*', note):
                note = SI
            else:
                raise Exception('sin concidencia para ' + note)

            divider = int(divider)
            if divider > 0:
                note_duration = wholenote / divider
            else:
                note_duration = wholenote / abs(divider)
                note_duration *= 1.5

            note_duration = int(note_duration)

            print("Note:", note, "Note duration: ", note_duration)

            first_byte = (note << 5) | (note_duration >> 8)
            second_byte = note_duration & 0b11111111

            sender.send_data(hex(first_byte).upper())
            sender.send_data(hex(second_byte).upper())

        # Print remaining bytes in buffer
        sender.send_data('0x0')
        print('\n', file=dest_file)

        print('bytes in table ', str_filename, ': ', sender._transmited_bytes, sep='')


def main():
    with open('../code/tonadas_autogeneradas.inc', 'w') as dest_file:

        with open('cabecera_tonadas.inc', 'r') as cabecera:
            print(cabecera.read(), file=dest_file)

        for index, str_filename in enumerate(glob.glob('*.tonada.txt')):
            generate_table(index, str_filename, dest_file)


if __name__ == '__main__':
    main()
