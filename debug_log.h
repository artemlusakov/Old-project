#ifndef DEBUG_LOG_H
#define DEBUG_LOG_H

typedef enum eagDebugLog_LineMode {
  LOG_LINE_DISABLE, // Линейный режим не используется. Выходные данные выводятся в том виде, в каком они были получены в gs Debug Stream sPrint
  LOG_LINE_ENABLE, // Включен линейный режим. За каждым вызовом gs Debug Stream sPrint будет следовать новая строка
  LOG_LINE_TIMESTAMP, // Включен режим временной метки. При каждом вызове gs Debug Stream sPrint в начале будет указана временная метка, а в конце - новая строка
}eDebugLog_LineMode;

void DebugLog_Init(eDebugLog_LineMode mode);

/**
 * Функция, подобная printf, для печати в отладочных потоках
 */
void gsDebugStreamsPrint(const char * fmt, ...) __attribute__((format (printf, 1, 2)));
void DebugLog_Export(void);

#endif  // #ifndef DEBUG_LOG_H
