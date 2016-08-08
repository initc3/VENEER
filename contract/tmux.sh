#!/bin/bash
tmux new-session    'bash' \;\
    splitw -h -p 50 'bash' \;\
    splitw -v -p 50 'bash' \;\
    select-pane -L         \;\
    splitw -v -p 50 'bash' \;\
    select-pane -U         \;\
    set -g mode-mouse on  \;\
    set -g mouse-resize-pane on  \;\
    set -g mouse-select-pane on  \;\
    set -g mouse-select-window on \;\
