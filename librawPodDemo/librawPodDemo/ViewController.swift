//
//  ViewController.swift
//  librawPodDemo
//
//  Created by meitu@nenhall on 2023/12/18.
//

import Cocoa

class ViewController: NSViewController {
    let raw = libraw_init(0)
    let pRaw = PixRaw();

    override func viewDidLoad() {
        super.viewDidLoad()


    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }


}

