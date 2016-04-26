<?php

namespace Drupal\paragraphs_extra\Widget;

interface WidgetInterface {

  /**
   * Get the type name of this widget.
   */
  public function getType();

  /**
   * Get the form element for this widget.
   */
  public function getElement();

  /**
   * Get the current widget values.
   */
  public function getValues();

  /**
   * Set the current widget values.
   */
  public function setValues(array $values);

  /**
   * Perform preprocessing of the field element.
   */
  public function init(array &$field_element);

  /**
   * Build form element.
   */
  public function element(array &$element, array $paragraphs);

  /**
   * Render widget.
   */
  public function render(array $variables, array $element);

  /**
   * Validate submitted values.
   */
  public function validate(array $element, array $values);

  /**
   * Handle submitted values after validated.
   */
  public function submit(\Drupal\paragraphs_extra\Widget\State $state, array $element, array $values);

  /**
   * Update widget based on current field widget state.
   */
  public function setState(\Drupal\paragraphs_extra\Widget\State $state);
}
