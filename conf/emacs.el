;;
;; AOS
;; (C) AlgoEngineering LLC 2008-2012
;; (C) NYSE | Intercontinental Exchange 2013-2014
;;
;; This is a commonly used .emacs file. To install:
;;
;;    ln -s ~/.emacs `pwd`/conf/emacs.el
;;    ln -s ~/elisp  `pwd`/conf/elisp
;;

(setq ffroot-directory default-directory)

(defun kill-current-buffer () (interactive)
  (kill-buffer (current-buffer)))


(defun insert-date ()
  "Insert current date in the format 'month date year' into the current buffer"
  (interactive)
  (insert (format-time-string "%b %d %Y" (current-time)))
  )

(defun insert-time ()
  "Insert current time in the format H:M:S into the current buffer"
  (interactive)
  (insert (format-time-string "%H:%M:%S" (current-time)))
  )


;; -----------------------------------------------------------------------------
;; separator line

(defun insert-separator ()
  "Insert separator using current mode's comment char"
  (interactive)
  (insert comment-start)
  (insert "-----------------------------------------------------------------------------\n"))

;;
;; global search for current word
;;
(defun ff-word ()
  "Invoke ff to find current word in the directory tree"
  (interactive)
  (ignore-errors (kill-buffer "*grep*"))
  (grep-find (concat "cd " ffroot-directory " && bin/ff -notty -g -n "
                     (shell-quote-argument (current-word)))))


;;
;; global search for current word
;;
(defun ff-word-with-generated-code ()
  "Invoke ff to find current word in the directory tree"
  (interactive)
  (ignore-errors (kill-buffer "*grep*"))
  (grep-find (concat "cd " ffroot-directory " && bin/ff -notty -g "
                     (shell-quote-argument (current-word)))))

;;
;; global search for current word
;;
(defun ff-try-to-find-function-definition ()
  "Invoke ff to find current word in the directory tree"
  (interactive)
  (ignore-errors (kill-buffer "*grep*"))
  (grep-find (concat "cd " ffroot-directory " && bin/ff -notty -g -n "
                     (shell-quote-argument (current-word))
                     ".*{")))

;;
;; tex-verbatim
;;
(defun tex-verbatim ()
  "Insert a verbatim block"
  (interactive)
  (point-to-register 1)
  (exchange-point-and-mark)
  (insert "\\begin{verbatim}\n")
  (jump-to-register 1)
  (insert "\\end{verbatim}\n")
  )

;;
;; jira-code
;;
(defun jira-code ()
  "Wrap region in jira code block"
  (interactive)
  (point-to-register 1)
  (exchange-point-and-mark)
  (insert "{code}\n")
  (jump-to-register 1)
  (insert "{code}\n")
  )

;;
;; jira-before-after
;;
(defun jira-before-after ()
  "Create a rewrite block"
  (interactive)
  (kill-ring-save (point) (mark))
  (point-to-register 1)
  (exchange-point-and-mark)
  (insert "Before:\n")
  (insert "{code}\n")
  (jump-to-register 1)
  (insert "{code}\n")
  (insert "After:\n")
  (insert "{code}\n")
  (yank)
  (insert "{code}\n")
  )

;;
;; consider _ as part of identifier
;; use font colors
;; use which-function-mode
;;
(defun customize-c-mode ()
  (modify-syntax-entry ?_ "w")
  (turn-on-font-lock)
  )

(add-hook 'c-mode-hook 'customize-c-mode)
(add-hook 'c++-mode-hook 'customize-c-mode)

(font-lock-add-keywords 'c++-mode
                        '(("\\<\\(fixme\\|FIXME\\|TODO\\):" 1 font-lock-warning-face prepend)
                          ("\\<\\(ind_beg\\|ind_end\\|frep_\\|frep1_\\|ssizeof\\|_array_count\\|vrfy\\|vrfy_\\)\\>" . font-lock-keyword-face)
                          ("\\<\\(void\\|int\\|bool\\)\\>" . font-lock-builtin-face)
                          ("\\<~\\|!\\>" . font-lock-negation-char-face)
                          ))

;;-----------------------------------------------------------------------------

;;
;; Define alignment on multiple columns
;;
(defun align-repeat (start end regexp)
  "Repeat alignment with respect to
     the given regular expression."
  (interactive "r\nsAlign regexp: ")
  (align-regexp start end
                (concat "\\(\\s-*\\)" regexp) 1 1 t))


;;
;; Define regexps for ssim attrs
;;
(defconst ssim-font-lock-keywords
  (list
   '(":\\([-/_a-zA-Z0-9\\.@]+\\)\\>" 1 font-lock-variable-name-face)
   '("\\<\\([-_a-zA-Z0-9\\.@]+\\):" 1 font-lock-function-name-face)
   '("^[ ]*\\([-a-zA-Z0-9_\\.@]+\\)[ ]" 1 font-lock-keyword-face)
   )
  "Minimal highlighting expressions for ssim mode")

;;
;; Allow underscore and - in words.
;;
(defvar ssim-mode-syntax-table
  (let ((st (make-syntax-table)))
    (modify-syntax-entry ?_ "w" st)
    (modify-syntax-entry ?- "w" st)
    st)
  "Syntax table for ssim-mode")

(defvar ssim-mode-hook nil)

(defun ssim-mode ()
  "Major mode for editing SSIM files"
  (interactive)
  (kill-all-local-variables)
  (set-syntax-table ssim-mode-syntax-table)
  (set (make-local-variable 'font-lock-defaults) '(ssim-font-lock-keywords))
  (setq major-mode 'ssim-mode)
  (setq mode-name "SSIM")
  (run-hooks 'ssim-mode-hook))

(provide 'ssim-mode)

;;-----------------------------------------------------------------------------

(setq auto-mode-alist
      (append '(("\\.C$"  . c++-mode)
                ("\\.cc$" . c++-mode)
                ("\\.cpp$" . c++-mode)
                ("\\.hh$" . c++-mode)
                ("\\.c$"  . c++-mode)
                ("\\.h$"  . c++-mode)
                ("\\.ssim$"  . ssim-mode)
                ("\\.md$"  . markdown-mode)
                ("\\.markdown$"  . markdown-mode)
                ) auto-mode-alist))

(autoload 'markdown-mode "markdown-mode"
  "Major mode for editing Markdown files" t)

(autoload 'gfm-mode "markdown-mode"
  "Major mode for editing GitHub Flavored Markdown files" t)

(custom-set-variables
 ;; custom-set-variables was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(cc-search-directories (quote ("." "/usr/include" "/usr/local/include/*")))
 '(grep-find-command (concat "cd " ffroot-directory " && bin/ff -notty -g -n "))
 '(send-mail-function (quote sendmail-send-it))
 '(warning-suppress-types (quote ((undo discard-info)))))

(setq default-frame-alist '((background-color . "black")
                            (foreground-color . "#ffd700")))

(custom-set-faces
 ;; custom-set-faces was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(default                          ((((class color) (min-colors 8)) (:foreground "#FFD900" :background "#000000"))))
 '(font-lock-keyword-face           ((((class color) (min-colors 8)) (:foreground "#A0C2FA"))))
 '(font-lock-comment-face           ((((class color) (min-colors 8)) (:foreground "#5faf5f"))))
 '(font-lock-comment-delimiter-face ((((class color) (min-colors 8)) (:foreground "#5DBA5F"))))
 '(font-lock-function-name-face     ((((class color) (min-colors 8)) (:foreground "#88CAEB"))))
 '(font-lock-builtin-face           ((((class color) (min-colors 8)) (:foreground "#63C1FF"))))
 '(font-lock-preprocessor-face      ((((class color) (min-colors 8)) (:foreground "#63C1FF"))))
 '(font-lock-variable-name-face     ((((class color) (min-colors 8)) (:foreground "#ffd787"))))
 '(font-lock-type-face              ((((class color) (min-colors 8)) (:foreground "#d7af87"))))
 '(font-lock-constant-name-face     ((((class color) (min-colors 8)) (:foreground "#63C1FF"))))
 '(font-lock-negation-char-face     ((((class color) (min-colors 8)) (:foreground "#00ffff"))))
 '(font-lock-string-face            ((((class color) (min-colors 8)) (:foreground "#7fff7f"))))
 '(diff-added                       ((((class color) (min-colors 8)) (:foreground "#7fff7f" :background "#222222"))))
 '(diff-removed                     ((((class color) (min-colors 8)) (:foreground "#ff7f7f" :background "#222222"))))
 '(diff-hunk-header                 ((((class color) (min-colors 8)) (:foreground "#A0C2FA" :background "#000000"))))
 '(diff-header                      ((((class color) (min-colors 8)) (:foreground "#A0C2FA" :background "#222222"))))
 '(diff-file-header                 ((((class color) (min-colors 8)) (:foreground "#63C1FF" :background "#222222"))))
 )

;;-------------------------------------------------------------------------------

(defun uniquify-lines-region (start end)
  "Find duplicate lines in region START to END keeping first occurrence."
  (interactive "*r")
  (save-excursion
    (let ((end (copy-marker end)))
      (while
          (progn
            (goto-char start)
            (re-search-forward "^\\(.*\\)\n\\(\\(.*\n\\)*\\)\\1\n" end t))
        (replace-match "\\1\n\\2")))))

(defun uniquify-lines-buffer ()
  "Delete duplicate lines in buffer and keep first occurrence."
  (interactive "*")
  (uniquify-lines-region (point-min) (point-max)))

;;-----------------------------------------------------------------------------
;;
;; These are useful to implement macros
;;
(defun increment-number-at-point ()
  (interactive)
  (skip-chars-backward "0123456789")
  (if (looking-at "[0123456789]+")
      (replace-match (number-to-string (+ (string-to-number (match-string 0)) 1)))))

(defun decrement-number-at-point ()
  (interactive)
  (skip-chars-backward "0123456789")
  (if (looking-at "[0123456789]+")
      (replace-match (number-to-string (- (string-to-number (match-string 0)) 1)))))

;-----------------------------------------------------------------------------

(defun trim-and-indent-region ()
  "Indent region, trimming any unnecessary whitespace"
  (interactive)
  (delete-trailing-whitespace)
  (indent-region (region-beginning) (region-end)))

;-----------------------------------------------------------------------------

(defun kill-and-recompile ()
  "Initiate compilation, ensuring that it is a fresh run (kill off previous process if necessary"
  (interactive)
  (if (not (eq (get-buffer-process "*compilation*") nil))
      (progn
        (ignore-errors
          (kill-compilation))
        (sit-for 1)))
  (save-some-buffers t)
 (recompile))

;; kill all windows. split window into 2. select compilation output
;; in lower pane. place cursor at the end of output (to watch progress)
;; then switch to other window, otherwise visiting an error causes emacs
;; to split the windows even more
;;
(fset 'compile-show-output
      "\C-x1\C-x2\C-xo\C-xb*comp\C-i\C-m\C-[>\C-xo")

;;-----------------------------------------------------------------------------
;; gnome work-around
;; what does this do exactly?

(define-key function-key-map "\eO1;2P" [(shift f1)])
(define-key function-key-map "\eO1;2Q" [(shift f2)])
(define-key function-key-map "\eO1;2R" [(shift f3)])
(define-key function-key-map "\eO1;2S" [(shift f4)])

(define-key function-key-map "\eO1;5P" [(control f1)])
(define-key function-key-map "\eO1;5Q" [(control f2)])
(define-key function-key-map "\eO1;5R" [(control f3)])
(define-key function-key-map "\eO1;5S" [(control f4)])

;;------------------------------------------------------------------------------

; strip 'Directory '
(setq compile-command              (concat "cd " ffroot-directory " && ai %"))

(setq-default c-basic-offset               4)
(setq-default indent-tabs-mode             nil)
(setq isearch-lazy-highlight       t)
(setq compilation-scroll-output    t)
(setq load-home-init-file          t)
(setq undo-limit                   1000000)
(setq undo-strong-limit            10000000)
(setq large-file-warning-threshold (* 50 1024 1024))
(setq split-width-threshold        nil)
(setq split-height-threshold       nil)
(setq inhibit-splash-screen        t)
(setq inverse-video                t)
(setq font-use-system-font         t)

(setq load-path (cons "~/elisp" load-path))
(show-paren-mode)
(column-number-mode    t)
(delete-selection-mode t)      ; typing over selection kills it first
(menu-bar-mode -1)             ; hide menu bar
(global-auto-revert-mode t)    ; reload files when they change on disk

(global-set-key (kbd "<f7>")          'kill-and-recompile)
(global-set-key (kbd "S-<f7>")        'kill-compilation)
(global-set-key (kbd "C-c C-k")        'kill-compilation)

;; visual studio's alt-2 command
(global-set-key (kbd "M-2")           'compile-show-output)
(global-set-key (kbd "<f2>")          'bm-next)
(global-set-key (kbd "C-<f2>")        'bm-bookmark-line)

(global-set-key (kbd "<f3>")          'ff-word)
(global-set-key (kbd "C-<f3>")        'ff-word-with-generated-code)
(global-set-key (kbd "<f2>")         'ff-try-to-find-function-definition)

(global-set-key (kbd "S-<f4>")        'previous-error)
(global-set-key (kbd "<f4>")          'next-error)
(global-set-key (kbd "C-<f4>")        'kill-current-buffer)

(global-set-key (kbd "C-<tab>")       'next-buffer)
(global-set-key (kbd "<backtab>")     'previous-buffer)

(global-set-key (kbd "M-<f8>")        'trim-and-indent-region)
(global-set-key (kbd "C-M-\\")        'trim-and-indent-region)

(global-set-key (kbd "C-<home>")      'beginning-of-buffer)
(global-set-key (kbd "C-<end>")       'end-of-buffer)
(global-set-key (kbd "C-<backspace>") 'backward-kill-word)
(global-set-key (kbd "C-<delete>")    'kill-word)
(global-set-key (kbd "C-<return>")    'electric-buffer-list)
(global-set-key (kbd "C-\\")          'bury-buffer)
(global-set-key (kbd "C-.")           'complete-symbol)
(global-set-key (kbd "M-o")           'algo-ff-toggle-source)
(global-set-key (kbd "M-p")           'algo-ff-open-other)
(global-set-key (kbd "C-%")           'query-replace-regexp)
(global-set-key (kbd "C-x /")         'insert-separator)
(global-set-key (kbd "<select>")      'end-of-line)

;; debugger commands
(global-set-key (kbd "<f11>")     'gud-step)   ;step into
(global-set-key (kbd "S-<f11>")   'gud-finish) ;step out of function
(global-set-key (kbd "<f10>")     'gud-next)   ;step over
(global-set-key (kbd "C-<f10>")   'gud-until)  ;go until current line
(global-set-key (kbd "<f12>")     'gdb-restore-windows)
(global-set-key (kbd "<f5>")      'gud-cont) ;run
(global-set-key (kbd "<f9>")      'gud-break) ;set breakpoint
(global-set-key (kbd "C-<f9>")    'gud-remove) ;clear breakpoint

(global-set-key (kbd "M-<f11>")   'gud-down)
(global-set-key (kbd "M-S-<f11>") 'gud-up)

(global-set-key (kbd "<f6>")      'other-window)

(global-set-key (kbd "C-c v")     'tex-verbatim)

(put 'downcase-region 'disabled nil)   ; kill alt-l

;; OK to kill buffer with process inside
(setq kill-buffer-query-functions
      (delq 'process-kill-buffer-query-function kill-buffer-query-functions))

;;-------------------------------------------------------------------------------
;;
;; find current file's basename, stripping _gen suffix and h, cpp, inl.h extensions
;; use ff to find similar files in the directory tree
;; construct a list of these filenames; cycle between these files
;;

(defun algo-ff-toggle-source ()
  (interactive)
  (find-file
   (shell-command-to-string
    (concat "cd " ffroot-directory "; bin/src_func -nextfile:"
            (buffer-file-name)))))

;;-----------------------------------------------------------------------------

(defun algo-ff-open-other ()
  (interactive)
  (find-file (shell-command-to-string (concat "cd " ffroot-directory "; bin/src_func -other -nextfile:" (buffer-file-name)))))

;;-----------------------------------------------------------------------------

;; Find e-mail for user USERNAME using acr lookup in acmdb.user table
;; Return the first found e-mail, being careful to strip trailing newlines
(defun email-of (username)
  (shell-command-to-string (concat "cd " ffroot-directory "; for X in $(bin/acr acmdb.user:" username " -field:email -report:N); do echo -n $X; exit 0; done")))

;;-----------------------------------------------------------------------------

;; Begin composing e-mail to user USERNAME
(defun email-to ()
  (interactive)
  (let ((username (read-string "Recipient username: " "" "" nil nil)))
    (compose-mail (email-of username))))

;;-----------------------------------------------------------------------------

(defun what-face (pos)
  (interactive "d")
  (let ((face (or (get-char-property (point) 'read-face-name)
                  (get-char-property (point) 'face))))
    (if face (message "Face: %s" face) (message "No face at %d" pos))))

;;--------------------------------------------------------------------------------

;; allow selecting a region and deleting it with backspace
(require 'delsel)

;; disable vc-git
;; http://stackoverflow.com/questions/5748814/how-does-one-disable-vc-git-in-emacs
(eval-after-load "vc" '(remove-hook 'find-file-hooks 'vc-find-file-hook))
