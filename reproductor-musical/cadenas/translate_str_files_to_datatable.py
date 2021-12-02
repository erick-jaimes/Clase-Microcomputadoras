# El código para las tablas de búsqueda ha sido sacado de:
# http://www.piclist.com/techref/microchip/tables.htm
import glob
import os


def remove_file_extension(filename):
    return os.path.splitext(filename)[0]


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
            # Las comillas inicialies/final no se consideran parte de los datos,
            # por lo que se eliminan de la cuenta
            # Por otra parte, los datos pueden traer comillas intermedias
            # escapadas con \. Visualmente aparecen como dos caracteres, pero
            # siguen siendo uno por lo que también se reduce uno a la cuenta
            # por cada comilla intermedia que aparezca.
            # En resumen, por cada comilla inicial/final o intermedia que se
            # encuentre en la cadena, restamos un byte a la longitud de los
            # datos para obtener el número de bytes que nos importan

            self._transmited_bytes += len(data) - data.count('"')
        else:
            # Las cadenas que no empiezan por comillas deben representar
            # un solo byte en formato hexadecimal. Ej: 0x27, 0xFF
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
    buffer_ascii_string = ''
    first_data_printed = True

    # Name of the subroutine
    subroutine_name = remove_file_extension(str_filename)
    print(f'''
{subroutine_name}:
    bcf STATUS, RP1
    bcf STATUS, RP0 ; Banco 0

    addlw LOW(str{index}__table)
    movwf (CADENAS__temp & 0x7F)
    rlf (CADENAS__zero & 0x7F), W
    addlw HIGH(str{index}__table)
    movwf PCLATH
    movf (CADENAS__temp & 0x7F), W
    movwf PCL
str{index}__table''', file=dest_file)

    sender = CommaSeparatedDataSender(dest_file)
    sender.table_directive()

    with open(str_filename, 'rb')  as str_file:

        for char_code in str_file.read():
            # If code is a printable 7 bit ascii, it is printed normally.
            # Otherwise, it is printed as a number

            if 32 <= char_code <= 126:
                buffer_ascii_string += chr(char_code)
            else:
                if buffer_ascii_string:
                    escaped_string = buffer_ascii_string.replace(r'"', r'\"')
                    sender.send_data(f'"{escaped_string}"')
                    buffer_ascii_string = ''

                if char_code == 10:
                    char_code = 13  # Para esta terminal virtual
                sender.send_data(hex(char_code).upper())

        # Print remaining bytes in buffer
        if buffer_ascii_string:
            sender.send_data(f'"{buffer_ascii_string}"')

        sender.send_data('0x0')
        print('\n', file=dest_file)

        print('bytes in table ', str_filename, ': ', sender._transmited_bytes, sep='')


def main():
    with open('../code/cadenas_autogeneradas.inc', 'w') as dest_file:

        with open('cabecera_cadenas.inc', 'r') as cabecera:
            print(cabecera.read(), file=dest_file)

        for index, str_filename in enumerate(glob.glob('str_*.txt')):
            generate_table(index, str_filename, dest_file)


if __name__ == '__main__':
    main()
