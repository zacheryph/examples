let rec hilo n =
  print_string "enter guess: ";
  let i = read_int () in
  if i = n then print_string "Bravo\n"
  else begin
    print_string (if i > n then "Lower\n" else "Higher\n");
    hilo n
  end

(* this is like main()
 *
 * technically it can be anything
 * i see let _ = a lot of places as well
 *)
let () = hilo 64 ;;
