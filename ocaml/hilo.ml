let rec hilo n =
  print_string "enter guess: ";
  let i = read_int () in
  if i = n then print_string "Bravo\n"
  else begin
    print_string (if i > n then "Lower\n" else "Higher\n");
    hilo n
  end

let () = hilo 64 ;;
